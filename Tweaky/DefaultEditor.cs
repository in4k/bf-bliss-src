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

namespace Tweaky
{
    /// <summary>
    /// Interaction logic for DefaultEditor.xaml
    /// </summary>
    public class DefaultEditor : EditorBase<object>
    {
        static DefaultEditor()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(DefaultEditor), new FrameworkPropertyMetadata(typeof(DefaultEditor)));
        }
    }
}
