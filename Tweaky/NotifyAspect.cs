using PostSharp.Aspects;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Tweaky
{
    [Serializable]
    [global::System.AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
    public sealed class NotifyAttribute : OnMethodBoundaryAspect
    {
        private List<string> additionalProperties = new List<string>();
        public NotifyAttribute()
        {
        }

        public NotifyAttribute(string additionalProperties)
        {
            if (additionalProperties != null)
            {
                var properties = additionalProperties.Split(',');
                this.additionalProperties.AddRange(properties);
            }
        }

        private string methodName;
        private Type className;
        private MethodInfo method;
        private MethodInfo getter;

        public override void RuntimeInitialize(System.Reflection.MethodBase method)
        {
            methodName = method.Name;
            className = method.DeclaringType;
            try
            {
                var methods = className.GetMethods();
                foreach (var m in methods)
                {
                    if (m.Name == "RaisePropertyChanged")
                    {
                        this.method = m;
                    }
                }
            }
            catch (Exception e)
            {
                if (Application.Current != null && Application.Current.MainWindow != null && !DesignerProperties.GetIsInDesignMode(Application.Current.MainWindow))
                    throw e;
            }
        }

        private object oldValue;
        public override void OnEntry(MethodExecutionArgs args)
        {
            if (args == null)
                return;

            if (null != getter)
                getter = args.Instance.GetType().GetMethod("get_" + methodName.Substring(4));

            if (null != getter)
                oldValue = getter.Invoke(args.Instance, null);
        }

        public override void OnExit(MethodExecutionArgs args)
        {
            if (args == null)
                return;

            if (null != method)
            {
                if (null != getter)
                {
                    var newValue = getter.Invoke(args.Instance, null);
                    if (newValue == null && oldValue == null || (newValue != null && newValue.Equals(oldValue)))
                        return;
                }

                method.Invoke(args.Instance, new object[] { methodName.Substring(4) });

                foreach (var prop in additionalProperties)
                    method.Invoke(args.Instance, new object[] { prop });
            }
        }
    }  
}
