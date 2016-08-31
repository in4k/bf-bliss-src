using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Tweaky.PropertyGrid
{
    public class PropertyItem : DependencyObject, INotifyPropertyChanged
    {
        public PropertyItem(object source, int index, PropertyDescriptor pd)
        {
            this.PropertyDescriptor = pd;
            this.Instance = source;
            this.Index = index;
            this.PropertyItems = new ObservableCollectionEx<PropertyItem>();
            if (source is INotifyPropertyChanged)
                (source as INotifyPropertyChanged).PropertyChanged += Source_PropertyChanged;

            var categoryAttribute = pd.Attributes.OfType<CategoryAttribute>().FirstOrDefault();
            var descriptionAttribute = pd.Attributes.OfType<DescriptionAttribute>().FirstOrDefault();
            var displayNameAttribute = pd.Attributes.OfType<DisplayNameAttribute>().FirstOrDefault();
            var readOnlyAttribute = pd.Attributes.OfType<ReadOnlyAttribute>().FirstOrDefault();
            Category = categoryAttribute == null ? "Misc" : categoryAttribute.Category;
            Description = descriptionAttribute == null ? "" : descriptionAttribute.Description;
            Name = displayNameAttribute == null ? pd.Name : displayNameAttribute.DisplayName;
            if (!pd.IsReadOnly && (readOnlyAttribute == null || !readOnlyAttribute.IsReadOnly))
            {
                IsReadOnly = false;
                BindingOperations.SetBinding(this, ValueProperty, new Binding() { Source = source, Path = new PropertyPath(pd.Name) });
            }
            else
            {
                IsReadOnly = true;
                BindingOperations.SetBinding(this, ValueProperty, new Binding() { Source = source, Path = new PropertyPath(pd.Name), Mode = BindingMode.OneWay });
            }
        }

        void Source_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == this.PropertyDescriptor.Name || string.IsNullOrEmpty(e.PropertyName))
                RaisePropertyChanged("Value");
        }

        public int Index
        {
            get;
            private set;
        }

        public PropertyItem Parent
        {
            get;
            private set;
        }

        public object Instance
        {
            get;
            private set;
        }

        public PropertyDescriptor PropertyDescriptor
        {
            get;
            private set;
        }

        public string Category
        {
            get;
            private set;
        }

        public string Description
        {
            get;
            private set;
        }

        public string Name
        {
            get;
            private set;
        }

        public bool IsReadOnly
        {
            get;
            private set;
        }

        public bool IsEditable
        {
            get
            {
                return !IsReadOnly;
            }
        }

        public bool IsExpanded
        {
            get;
            [Notify]
            set;
        }

        public bool IsSelected
        {
            get;
            [Notify]
            set;
        }

        public object Value
        {
            get { return this.GetValue(ValueProperty); }
            [Notify]
            set { this.SetValue(ValueProperty, value); }
        }
        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(object), typeof(PropertyItem), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, OnValueChanged));

        private static void OnValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var pi = (d as PropertyItem);
            pi.RaisePropertyChanged("Value");
            (pi.Instance as ViewModelBase).RaisePropertyChanged("Name");
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        public ObservableCollectionEx<PropertyItem> PropertyItems
        {
            get;
            private set;
        }
    }
}
