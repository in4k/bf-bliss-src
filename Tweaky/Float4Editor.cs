using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Tweaky.PropertyGrid;

namespace Tweaky
{
    public class Float4Editor : EditorBase<float4>
    {
        static Float4Editor()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(Float4Editor), new FrameworkPropertyMetadata(typeof(Float4Editor)));
        }

        public override void OnValueChanged()
        {
            base.OnValueChanged();

            RaisePropertyChanged("X");
            RaisePropertyChanged("Y");
            RaisePropertyChanged("Z");
            RaisePropertyChanged("W");
        }

        public float Minimum
        {
            get
            {
                var pi = this.DataContext as PropertyItem;
                var rangeAttribute = pi.PropertyDescriptor.Attributes.OfType<RangeAttribute>().FirstOrDefault();
                if (rangeAttribute == null)
                    return 0.0f;

                return rangeAttribute.Minimum;
            }
        }

        public float Maximum
        {
            get
            {
                var pi = this.DataContext as PropertyItem;
                var rangeAttribute = pi.PropertyDescriptor.Attributes.OfType<RangeAttribute>().FirstOrDefault();
                if (rangeAttribute == null)
                    return 1.0f;

                return rangeAttribute.Maximum;
            }
        }

        public float StepHint
        {
            get
            {
                var pi = this.DataContext as PropertyItem;
                var rangeAttribute = pi.PropertyDescriptor.Attributes.OfType<RangeAttribute>().FirstOrDefault();
                if (rangeAttribute == null)
                    return 0.1f;

                return rangeAttribute.StepHint;
            }
        }

        public float X
        {
            get { return Value.x; }
            set { Value = new float4(value, Value.y, Value.z, Value.w); }
        }

        public float Y
        {
            get { return Value.y; }
            set { Value = new float4(Value.x, value, Value.z, Value.w); }
        }

        public float Z
        {
            get { return Value.z; }
            set { Value = new float4(Value.x, Value.y, value, Value.w); }
        }

        public float W
        {
            get { return Value.w; }
            set { Value = new float4(Value.x, Value.y, Value.z, value); }
        }
    }
}
