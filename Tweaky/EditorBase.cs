using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using Tweaky.PropertyGrid;

namespace Tweaky
{
    public class EditorBase<T> : Control, INotifyPropertyChanged
    {
        static EditorBase()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(EditorBase<T>), new FrameworkPropertyMetadata(typeof(EditorBase<T>)));
        }

        public EditorBase()
        {
            this.DataContextChanged += EditorBase_DataContextChanged;
        }

        void EditorBase_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (e.OldValue != null && e.OldValue is PropertyItem)
                (e.OldValue as PropertyItem).PropertyChanged -= EditorBase_PropertyChanged;

            if (e.NewValue != null && e.NewValue is PropertyItem)
            {
                (e.NewValue as PropertyItem).PropertyChanged -= EditorBase_PropertyChanged;
                (e.NewValue as PropertyItem).PropertyChanged += EditorBase_PropertyChanged;
            }
        }

        void EditorBase_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Value")
                OnValueChanged();
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        public virtual void OnValueChanged()
        {
            RaisePropertyChanged(null);
        }

        public float Step
        {
            get
            {
                return 1.0f;
            }
        }

        public virtual T Value
        {
            get 
            {
                if (this.DataContext is PropertyItem)
                    return (T)(this.DataContext as PropertyItem).Value;

                return default(T);
            }
            set 
            {
                if (this.DataContext is PropertyItem)
                {
                    (this.DataContext as PropertyItem).Value = value; 
                    OnValueChanged(); 
                }
            }
        }
    }
}
