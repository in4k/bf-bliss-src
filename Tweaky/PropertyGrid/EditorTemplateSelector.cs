using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Reflection;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Tweaky.PropertyGrid
{
    public class EditorTemplateSelector : DataTemplateSelector
    {
        public EditorTemplateSelector()
        {
            defaultDataTemplate = new DataTemplate() { VisualTree = new FrameworkElementFactory(typeof(DefaultEditor)) };
        }

        private DataTemplate defaultDataTemplate;
        public override DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
        {
            if (item == null)
                return defaultDataTemplate;

            if (!(item is PropertyItem))
                return defaultDataTemplate;

            var pd = (item as PropertyItem).PropertyDescriptor;
            var editorAttribute = pd.Attributes.OfType<EditorAttribute>().LastOrDefault();

            if (editorAttribute != null)
            {
                var editorType = Type.GetType(editorAttribute.EditorTypeName);
                if (typeof(FrameworkElement).IsAssignableFrom(editorType))
                    return new DataTemplate() { VisualTree = new FrameworkElementFactory(editorType) };
            }

            if (pd.PropertyType.IsEnum)
            {
                return new DataTemplate() { VisualTree = new FrameworkElementFactory(typeof(EnumEditor)) };
            }
            else if (pd.PropertyType == typeof(System.Drawing.Color))
            {
                return new DataTemplate() { VisualTree = new FrameworkElementFactory(typeof(FloatArrayColorEditor)) };
            }
            else if (pd.PropertyType == typeof(float4))
            {
                return new DataTemplate() { VisualTree = new FrameworkElementFactory(typeof(Float4Editor)) };
            }

            return defaultDataTemplate;
        }
    }
}
