using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Tweaky.PropertyGrid;

namespace Tweaky
{
    /// <summary>
    /// Interaction logic for EnumEditor.xaml
    /// </summary>
    public class EnumEditor : EditorBase<System.Enum>
    {
        static EnumEditor()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(EnumEditor), new FrameworkPropertyMetadata(typeof(EnumEditor)));
        }

        public virtual Array Items
        {
            get
            {
                if (this.DataContext == null)
                    return new object[0];

                var pi = this.DataContext as PropertyItem;
                return Enum.GetValues(pi.PropertyDescriptor.PropertyType);
            }
        }
    }
}
