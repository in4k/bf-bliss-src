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
    public sealed class ExecuteAfterAttribute : OnMethodBoundaryAspect
    {
        public ExecuteAfterAttribute(string methodName)
        {
            this.methodName = methodName;
        }

        private string methodName;
        private Type className;
        private MethodInfo method;

        public override void RuntimeInitialize(System.Reflection.MethodBase method)
        {
            className = method.DeclaringType;
            try
            {
                var methods = className.GetMethods();
                foreach (var m in methods)
                {
                    if (m.Name == methodName)
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

        public override void OnExit(MethodExecutionArgs args)
        {
            if (args == null)
                return;

            if (null != method)
            {
                method.Invoke(args.Instance, null);
            }
        }
    }  
}
