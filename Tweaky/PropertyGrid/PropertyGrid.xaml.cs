using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Tweaky.PropertyGrid
{
    /// <summary>
    /// Interaction logic for PropertyGrid.xaml
    /// </summary>
    public partial class PropertyGrid : UserControl, INotifyPropertyChanged
    {
        public PropertyGrid()
        {
            InitializeComponent();
            PropertyItems = new ObservableCollection<PropertyItem>();
            LeftColumn = new GridLength(150, GridUnitType.Pixel);
            RightColumn = new GridLength(1, GridUnitType.Star);
        }

        public ObservableCollection<PropertyItem> PropertyItems
        {
            get;
            private set;
        }

        public GridLength LeftColumn
        {
            get;
            [Notify]
            set;
        }

        public GridLength RightColumn
        {
            get;
            [Notify]
            set;
        }

        private string filter;
        public string Filter
        {
            get
            {
                return filter;
            }
            [Notify]
            set
            {
                filter = value;
                (this.Resources["collectionView"] as CollectionViewSource).View.Refresh();
            }
        }
        
        public PropertyItem SelectedPropertyItem
        {
            get { return (PropertyItem)this.GetValue(SelectedPropertyItemProperty); }
            [Notify]
            set { this.SetValue(SelectedPropertyItemProperty, value); }
        }
        public static readonly DependencyProperty SelectedPropertyItemProperty = DependencyProperty.Register("SelectedPropertyItem", typeof(PropertyItem), typeof(PropertyGrid), new FrameworkPropertyMetadata(null));

        public string ItemsSourceType
        {
            get { return (string)this.GetValue(ItemsSourceTypeProperty); }
            [Notify]
            set { this.SetValue(ItemsSourceTypeProperty, value); }
        }
        public static readonly DependencyProperty ItemsSourceTypeProperty = DependencyProperty.Register("ItemsSourceType", typeof(string), typeof(PropertyGrid), new FrameworkPropertyMetadata(null));

        public string ItemsSourceName
        {
            get { return (string)this.GetValue(ItemsSourceNameProperty); }
            [Notify]
            set { this.SetValue(ItemsSourceNameProperty, value); }
        }
        public static readonly DependencyProperty ItemsSourceNameProperty = DependencyProperty.Register("ItemsSourceName", typeof(string), typeof(PropertyGrid), new FrameworkPropertyMetadata(null));

        public object ItemsSource
        {
            get { return this.GetValue(ItemsSourceProperty); }
            [Notify]
            set { this.SetValue(ItemsSourceProperty, value); }
        }
        public static readonly DependencyProperty ItemsSourceProperty = DependencyProperty.Register("ItemsSource", typeof(object), typeof(PropertyGrid), new FrameworkPropertyMetadata(null, OnItemsSourceChanged));

        private static void OnItemsSourceChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var me = d as PropertyGrid;
            if (me != null)
            {
                if (e.OldValue != null && e.OldValue is INotifyPropertyChanged)
                    (e.OldValue as INotifyPropertyChanged).PropertyChanged -= me.PropertyGrid_PropertyChanged;

                UpdatePropertyItems(me.ItemsSource, me.PropertyItems);
                me.RaisePropertyChanged(null);

                if (e.NewValue != null && e.NewValue is INotifyPropertyChanged)
                    (e.NewValue as INotifyPropertyChanged).PropertyChanged += me.PropertyGrid_PropertyChanged;
            }
        }

        void PropertyGrid_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Properties")
            {
                UpdatePropertyItems(ItemsSource, PropertyItems);
                RaisePropertyChanged(null);
            }
        }

        public static void UpdatePropertyItems(object itemsSource, IList<PropertyItem> propertyItems)
        {
            propertyItems.Clear();
            int index = 0;
            if (itemsSource != null)
            {
                var properties = (itemsSource is ICustomTypeDescriptor) ? (itemsSource as ICustomTypeDescriptor).GetProperties() : TypeDescriptor.GetProperties(itemsSource);
                foreach (PropertyDescriptor pd in properties.Cast<PropertyDescriptor>())
                {
                    var browsableAttribute = pd.Attributes.OfType<BrowsableAttribute>().FirstOrDefault();
                    if (browsableAttribute != null && !browsableAttribute.Browsable)
                        continue;

                    propertyItems.Add(new PropertyItem(itemsSource, index++, pd));
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        private void CollectionViewSource_Filter(object sender, FilterEventArgs e)
        {
            e.Accepted = string.IsNullOrEmpty(this.Filter) || (e.Item as PropertyItem).Name.ToLower().Contains(this.Filter.ToLower());
        }

        public void PropertyItem_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            var pi = (sender as FrameworkElement).DataContext as PropertyItem;
            if (this.SelectedPropertyItem == pi)
                return;

            if (pi != null)
            {
                if (this.SelectedPropertyItem != null)
                    this.SelectedPropertyItem.IsSelected = false;

                pi.IsSelected = true;
                this.SelectedPropertyItem = pi;
            }
        }
    }
}
