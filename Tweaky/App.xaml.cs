using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace Tweaky
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
            this.DispatcherUnhandledException += App_DispatcherUnhandledException;
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionHandler;
        }

        private void UnhandledExceptionHandler(object sender, UnhandledExceptionEventArgs e)
        {
            try
            {
                ShowException(e.ExceptionObject as Exception);
                this.Shutdown();
            }
            catch
            {
            }
        }

        void App_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            try
            {
                ShowException(e.Exception);
                this.Shutdown();
            }
            catch
            {
            }
        }

        private static void ShowException(Exception exception)
        {
            var message = exception.Message + " at:\n" + exception.StackTrace + "\n\n";
            do
            {
                exception = exception.InnerException;
                if (exception != null)
                    message += exception.Message + " at:\n" + exception.StackTrace + "\n\n";
            }
            while (exception != null);
            System.Windows.Forms.MessageBox.Show(message, "Unhandled exception");
        }
    }
}
