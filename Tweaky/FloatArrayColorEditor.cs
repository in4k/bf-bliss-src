using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Media;
using Tweaky.PropertyGrid;

namespace Tweaky
{
    public class FloatArrayColorEditor : ColorEditorBase<float>
    {
        static FloatArrayColorEditor()
        {
        }

        public override float Red
        {
            get
            {
                return Value.R / 255.0f;
            }
            set
            {
                UpdateColorFromRGB(value, Green, Blue);
            }
        }

        public override float Green
        {
            get
            {
                return Value.G / 255.0f;
            }
            set
            {
                UpdateColorFromRGB(Red, value, Blue);
            }
        }

        public override float Blue
        {
            get
            {
                return Value.B / 255.0f;
            }
            set
            {
                UpdateColorFromRGB(Red, Green, value);
            }
        }

        public override float Alpha
        {
            get
            {
                if (!UseAlpha)
                    return 0.0f;

                return Value.A / 255.0f;
            }
            set
            {
                if (!UseAlpha)
                    return;

                Values = new float[]
                {
                    value,
                    Red,
                    Green,
                    Blue
                };
                UpdateColorFromHSV(hue, sat, val);
            }
        }

        private bool colorInitialized;
        protected override void UpdateColorFromHSV(double hue, double sat, double val)
        {
            var rgb = HSVtoRGB(hue, sat, val);
            Values = new float[] { Alpha, (float)rgb[0], (float)rgb[1], (float)rgb[2] };
            BackgroundBrush = new SolidColorBrush(Color.FromArgb(Value.A, Value.R, Value.G, Value.B));
            if (!colorInitialized)
            {
                colorInitialized = true;
                ExternalBackgroundBrush = new SolidColorBrush(Color.FromArgb(Value.A, Value.R, Value.G, Value.B));
            }
            rgb = HSVtoRGB(hue, 1.0, 1.0);
            HueBrush = new SolidColorBrush(Color.FromArgb(255, (byte)(255 * rgb[0]), (byte)(255 * rgb[1]), (byte)(255 * rgb[2])));
            RaisePropertyChanged(null);
        }

        protected override void UpdateColorFromRGB(float red, float green, float blue)
        {
            var hsv = RGBtoHSV(red, green, blue);
            hue = hsv[0];
            sat = hsv[1];
            val = hsv[2];
            UpdateColorFromHSV(hue, sat, val);
        }

        protected override void UpdateColor(System.Drawing.Color color)
        {
            UpdateColorFromRGB(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f);
        }

        protected override void InitializeFromExternalColor()
        {
            var hsv = RGBtoHSV(Red, Green, Blue);
            hue = hsv[0];
            sat = hsv[1];
            val = hsv[2];
            UpdateColorFromHSV(hue, sat, val);
        }

        protected override System.Drawing.Color GetColorFromValues(float[] values)
        {
            return System.Drawing.Color.FromArgb((int)(values[0] * 255.0f), (int)(values[1] * 255.0f), (int)(values[2] * 255.0f), (int)(values[3] * 255.0f));
        }
    }
}
