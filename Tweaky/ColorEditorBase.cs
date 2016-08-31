using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using Tweaky.PropertyGrid;

namespace Tweaky
{
    [TemplatePart(Name = "PART_PopupButton", Type = typeof(Button))]
    [TemplatePart(Name = "PART_Popup", Type = typeof(Popup))]
    [TemplatePart(Name = "PART_SatValImage", Type = typeof(Border))]
    [TemplatePart(Name = "PART_HueImage", Type = typeof(Image))]
    [TemplatePart(Name = "PART_OriginalColorBorder", Type = typeof(Border))]
    [TemplatePart(Name = "HGradientBorder", Type = typeof(Border))]
    public class ColorEditorBase : EditorBase<System.Drawing.Color>
    {
        public ColorEditorBase()
        {
            this.LayoutUpdated += ColorEditorBase_LayoutUpdated;
        }

        void ColorEditorBase_LayoutUpdated(object sender, EventArgs e)
        {
            RaisePropertyChanged(null);
        }

        static ColorEditorBase()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ColorEditorBase), new FrameworkPropertyMetadata(typeof(ColorEditorBase)));
        }

        protected ToggleButton PopupButton;
        protected Popup Popup;
        protected Border SatValImage;
        protected Image HueImage;
        protected Border OriginalColorBorder;
        protected Border HGradientBorder;

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.OriginalColorBorder = this.Template.FindName("PART_OriginalColorBorder", this) as Border;
            this.PopupButton = this.Template.FindName("PART_PopupButton", this) as ToggleButton;
            this.SatValImage = this.Template.FindName("PART_SatValImage", this) as Border;
            this.HueImage = this.Template.FindName("PART_HueImage", this) as Image;
            this.HGradientBorder = this.Template.FindName("HGradientBorder", this) as Border;

            RaisePropertyChanged(null);
        }
    }   

    public abstract class ColorEditorBase<T> : ColorEditorBase
    {
        static ColorEditorBase()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ColorEditorBase<T>), new FrameworkPropertyMetadata(typeof(ColorEditorBase<T>)));
        }

        public ColorEditorBase()
        {
            UseAlpha = true;
        }

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            OriginalColorBorder.Cursor = new Cursor(Application.GetResourceStream(new Uri("pack://application:,,,/Tweaky;component/colorpicker.cur")).Stream);
            OriginalColorBorder.PreviewMouseDown += OriginalColorBorder_MouseDown;

            SatValImage.MouseDown += SatVal_MouseDown;
            SatValImage.MouseMove += SatVal_MouseMove;
            SatValImage.MouseUp += SatVal_MouseUp;

            HGradientBorder.MouseDown += Hue_MouseDown;
            HGradientBorder.MouseMove += Hue_MouseMove;
            HGradientBorder.MouseUp += Hue_MouseUp;

            InitializeFromExternalColor();

            this.LayoutUpdated += ColorEditorBase_LayoutUpdated;
        }

        void ColorEditorBase_LayoutUpdated(object sender, EventArgs e)
        {
            RaisePropertyChanged("HueIndicatorPosition");
        }

        public CustomPopupPlacement[] placePopup(Size popupSize, Size targetSize, Point offset)
        {
            CustomPopupPlacement placement1 = new CustomPopupPlacement(new Point(targetSize.Width - popupSize.Width, targetSize.Height), PopupPrimaryAxis.Horizontal);
            CustomPopupPlacement placement2 = new CustomPopupPlacement(new Point(0, targetSize.Height), PopupPrimaryAxis.Horizontal);
            CustomPopupPlacement[] ttplaces = new CustomPopupPlacement[] { placement1, placement2 };
            return ttplaces;
        }

        public abstract T Red { get; set; }
        public abstract T Green { get; set; }
        public abstract T Blue { get; set; }
        public abstract T Alpha { get; set; }

        protected double hue;
        public double Hue
        {
            get
            {
                return hue;
            }
            set
            {
                hue = value;
                UpdateColorFromHSV(hue, sat, val);
            }
        }

        protected double sat;
        public double Sat
        {
            get
            {
                return sat;
            }
            set
            {
                sat = value;
                UpdateColorFromHSV(hue, sat, val);
            }
        }

        protected double val;
        public double Val
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                UpdateColorFromHSV(hue, sat, val);
            }
        }

        public Brush BackgroundBrush
        {
            get;
            protected set;
        }

        protected abstract void UpdateColorFromHSV(double hue, double sat, double val);
        protected abstract void UpdateColorFromRGB(T red, T green, T blue);
        protected abstract void UpdateColor(System.Drawing.Color color);

        public Brush ExternalBackgroundBrush
        {
            get;
            protected set;
        }

        public Brush HueBrush
        {
            get;
            protected set;
        }

        public T[] Values
        {
            get
            {
                return new T[]
                {
                    Alpha,
                    Red,
                    Green,
                    Blue
                };
            }
            set
            {
                Value = GetColorFromValues(value);
            }
        }

        protected abstract System.Drawing.Color GetColorFromValues(T[] value);

        protected static System.Drawing.Color ColorFromHex(string value)
        {
            var converter = new System.Drawing.ColorConverter();
            if (value == null)
                return System.Drawing.Color.White;
            return (System.Drawing.Color)converter.ConvertFromString(null, System.Globalization.CultureInfo.InvariantCulture, value);
        }

        public string Hex
        {
            get
            {
                return ColorToHex(Value);
            }
            set
            {
                try
                {
                    Value = ColorFromHex(value);
                    UpdateColor(Value);
                }
                catch (Exception)
                {
                }
            }
        }

        public override System.Drawing.Color Value
        {
            get 
            {
                return GetColorFromValue(); 
            }
            set 
            {
                SetColorFromValue(value); 
                
                OnValueChanged(); 
            }
        }

        protected virtual void SetColorFromValue(System.Drawing.Color value)
        {
            (this.DataContext as PropertyItem).Value = value;
        }

        protected virtual System.Drawing.Color GetColorFromValue()
        {
            if (!(this.DataContext is PropertyItem))
                return System.Drawing.Color.White;

            if ((this.DataContext as PropertyItem).Value == null)
                return System.Drawing.Color.White;

            if ((this.DataContext as PropertyItem).Value is System.Drawing.Color)
                return (System.Drawing.Color)(this.DataContext as PropertyItem).Value;

            return System.Drawing.Color.White;                    
        }

        public override void OnValueChanged()
        {
            base.OnValueChanged();

            RaisePropertyChanged(null);
        }

        protected string ColorToHex(System.Drawing.Color value)
        {
            var values = new byte[] { value.A, value.R, value.G, value.B };
            return "#" + BitConverter.ToString(values).Replace("-", string.Empty);
        }

        protected bool useAlpha;
        public bool UseAlpha
        {
            get
            {
                return useAlpha;
            }
            set
            {
                if (useAlpha == value)
                    return;

                useAlpha = value;
                RaisePropertyChanged("UseAlpha");
                RaisePropertyChanged("AlphaGuiVisibility");
            }
        }

        public Visibility AlphaGuiVisibility
        {
            get
            {
                return useAlpha ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        protected double[] RGBtoHSV(double r, double g, double b)
        {
            double h, s, v;
            double min, max, delta;
            min = Math.Min(r, Math.Min(g, b));
            max = Math.Max(r, Math.Max(g, b));
            v = max;
            delta = max - min;
            if (max == 0 || delta == 0)
            {
                s = 0;
                h = 0;
            }
            else
            {
                s = delta / max;
                if (r == max)
                    h = (g - b) / delta;
                else if (g == max)
                    h = 2 + (b - r) / delta;
                else
                    h = 4 + (r - g) / delta;
                h *= 60;
                if (h < 0)
                    h += 360;
            }

            return new double[] { h, s, v };
        }

        protected double[] HSVtoRGB(double h, double s, double v)
        {
            int i;
            double r, g, b;
            double f, p, q, t;
            if (s == 0)
            {
                r = g = b = v;
            }
            else
            {
                h /= 60;
                i = (int)Math.Floor(h);
                f = h - i;
                p = v * (1 - s);
                q = v * (1 - s * f);
                t = v * (1 - s * (1 - f));
                switch (i)
                {
                    case 0:
                        r = v;
                        g = t;
                        b = p;
                        break;
                    case 1:
                        r = q;
                        g = v;
                        b = p;
                        break;
                    case 2:
                        r = p;
                        g = v;
                        b = t;
                        break;
                    case 3:
                        r = p;
                        g = q;
                        b = v;
                        break;
                    case 4:
                        r = t;
                        g = p;
                        b = v;
                        break;
                    default: // case 5:
                        r = v;
                        g = p;
                        b = q;
                        break;
                }
            }
            return new double[] { r, g, b };
        }

        protected void Button_Click(object sender, RoutedEventArgs e)
        {
            Popup.IsOpen = true;
        }

        protected bool hueDown = false;
        protected bool satValDown = false;

        protected void Hue_MouseDown(object sender, MouseButtonEventArgs e)
        {
            HGradientBorder.CaptureMouse();
            hueDown = true;
            UpdateHue(e.GetPosition(HGradientBorder));
        }

        protected void UpdateHue(Point point)
        {
            if (!hueDown)
                return;

            Hue = Math.Max(0, Math.Min(360.0, 360.0 * (point.X / HGradientBorder.ActualWidth)));
        }

        protected void Hue_MouseMove(object sender, MouseEventArgs e)
        {
            UpdateHue(e.GetPosition(HGradientBorder));
        }

        protected void Hue_MouseUp(object sender, MouseButtonEventArgs e)
        {
            hueDown = false;
            HGradientBorder.ReleaseMouseCapture();
        }

        protected void SatVal_MouseDown(object sender, MouseButtonEventArgs e)
        {
            SatValImage.CaptureMouse();
            satValDown = true;
            UpdateSatVal(e.GetPosition(SatValImage));
        }

        protected void UpdateSatVal(Point point)
        {
            if (!satValDown)
                return;

            Sat = Math.Max(0, Math.Min(1, point.X / SatValImage.ActualWidth));
            Val = Math.Max(0, Math.Min(1, 1.0 - (point.Y / SatValImage.ActualHeight)));
        }

        protected void SatVal_MouseMove(object sender, MouseEventArgs e)
        {
            UpdateSatVal(e.GetPosition(SatValImage));
        }

        protected void SatVal_MouseUp(object sender, MouseButtonEventArgs e)
        {
            satValDown = false;
            SatValImage.ReleaseMouseCapture();
        }

        public double HueIndicatorPosition
        {
            get
            {
                return Hue * (HueImage == null ? 1 : HueImage.ActualWidth) / 360.0;
            }
        }

        public double SatIndicatorPosition
        {
            get
            {
                return Sat * (SatValImage == null ? 1 : SatValImage.ActualWidth);
            }
        }

        public double ValIndicatorPosition
        {
            get
            {
                return (1.0 - Val) * (SatValImage == null ? 1 : SatValImage.ActualHeight);
            }
        }

        protected abstract void InitializeFromExternalColor();

        protected void OriginalColorBorder_MouseDown(object sender, MouseButtonEventArgs e)
        {
            InitializeFromExternalColor();
        }
    }
}
