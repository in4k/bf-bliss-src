using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace Tweaky
{
    public struct float3
    {
        public float x, y, z;
    };

    public struct float4
    {
        public float4(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public float x, y, z, w;
    };

    public struct TweakBufferType
    {
        public float4
        _fN, // _f Night
        _fD, // _f Day
        _r1N, // grade Lift Night
        _r1D, // grade Lift Day
        _r2N, // grade Gamma Night
        _r2D, // grade Gamma Day
        _r3N, // grade Gain Night
        _r3D, // grade Gain Day
        _lN, // _f Night
        _lD; // _f Day
    };

    public class DataViewModel : ViewModelBase, IDisposable
    {
        Timer timer;
        string path;

        unsafe public DataViewModel(string path)
        {
            this.path = path;
            mem = new SharedMemory<TweakBufferType>(path, Marshal.SizeOf(typeof(TweakBufferType)));
            mem.Open();

            using (var stream = new FileStream(path + ".bin", FileMode.Open))
            {
                var buffer = new byte[Marshal.SizeOf(typeof(TweakBufferType))];
                stream.Read(buffer, 0, buffer.Length);
                fixed (byte* ptr = &buffer[0])
                {
                    TweakBufferType* tweakBuffer = (TweakBufferType*)ptr;
                    mem.Data = tweakBuffer[0];
                }
            }

            timer = new Timer();
            timer.Interval = 1000;
            timer.Elapsed += timer_Elapsed;
            timer.Start();
        }

        void timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            if (dirty)
            {
                dirty = false;
                Save();
            }
        }

        [Category("Night")]
        public Color FogNight
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._fN);
            }
            set
            {
                var data = mem.Data;
                data._fN = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Day")]
        public Color FogDay
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._fD);
            }
            set
            {
                var data = mem.Data;
                data._fD = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Night")]
        [DisplayName("Color Grading: Lift")]
        public Color GradeLiftNight
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r1N);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r1N = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Day")]
        [DisplayName("Color Grading: Lift")]
        public Color GradeLiftDay
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r1D);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r1D = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Night")]
        [DisplayName("Color Grading: Gamma")]
        public Color GradeGammaNight
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r2N);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r2N = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Day")]
        [DisplayName("Color Grading: Gamma")]
        public Color GradeGammaDay
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r2D);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r2D = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Night")]
        [DisplayName("Color Grading: Gain")]
        public Color GradeGainNight
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r3N);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r3N = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Day")]
        [DisplayName("Color Grading: Gain")]
        public Color GradeGainDay
        {
            get
            {
                var data = mem.Data;
                return CreateColor(data._r3D);
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._r3D = ConvertColor(value);
                mem.Data = data;
            }
        }

        [Category("Night")]
        [DisplayName("Light direction")]
        [Range(-1, 1, 0.1f)]
        public float4 LightDirectionNight
        {
            get
            {
                var data = mem.Data;
                return data._lN;
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._lN = value;
                mem.Data = data;
            }
        }

        [Category("Day")]
        [DisplayName("Light direction")]
        [Range(-1, 1, 0.1f)]
        public float4 LightDirectionDay
        {
            get
            {
                var data = mem.Data;
                return data._lD;
            }
            [ExecuteAfter("MarkDirty")]
            set
            {
                var data = mem.Data;
                data._lD = value;
                mem.Data = data;
            }
        }

        private static Color CreateColor(float4 val)
        {
            return Color.FromArgb((int)(val.w * 255), (int)(val.x * 255), (int)(val.y * 255), (int)(val.z * 255));
        }

        private static float4 ConvertColor(Color val)
        {
            return new float4() { x = val.R / 255.0f, y = val.G / 255.0f, z = val.B / 255.0f, w = val.A / 255.0f };
        }

        private SharedMemory<TweakBufferType> mem;

        public void Dispose()
        {
            Save();

            mem.Close();
        }

        private bool dirty = false;
        public void MarkDirty()
        {
            this.dirty = true;
        }

        unsafe private void Save()
        {
            var buffer = new byte[Marshal.SizeOf(typeof(TweakBufferType))];

            using (var stream = new FileStream(path + ".bin", FileMode.Create))
            {
                fixed (byte* ptr = &buffer[0])
                {
                    TweakBufferType* tweakBuffer = (TweakBufferType*)ptr;
                    tweakBuffer[0] = mem.Data;
                }
                stream.Write(buffer, 0, buffer.Length);
            }

            using (var stream = new FileStream(path + ".h", FileMode.Create))
            {
                using (var writer = new StreamWriter(stream))
                {
                    writer.Write("#pragma once\n\n");
                    writer.Write("#define " + path.ToUpper() + "_SIZE " + buffer.Length + "\n\n");
                    writer.Write("#pragma data_seg(\"." + path + "\")\n");
                    writer.Write("static unsigned char " + path + "[] = {\n\t");
                    foreach (var b in buffer)
                    {
                        writer.Write("" + b + ", ");
                    }
                    writer.Write("\n};\n");
                }
            }
        }
    }
}
