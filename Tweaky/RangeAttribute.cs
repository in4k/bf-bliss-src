using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tweaky
{
    public class RangeAttribute : Attribute
    {
        public RangeAttribute(float minimum, float maximum, float stepHint)
        {
            this.Minimum = minimum;
            this.Maximum = maximum;
            this.StepHint = stepHint;
        }

        public float Minimum { get; private set; }
        public float Maximum { get; private set; }
        public float StepHint { get; private set; }
    }
}
