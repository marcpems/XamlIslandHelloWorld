//using Microsoft.UI.Xaml.Media;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace XamlIslandHelloWorld
{
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("8cf564c9-2017-4ee2-a95e-203b179a12cd")]
    public interface IBroker
    {
        void Initialize(INativeCallback callback);
        UIElement GetUI();
        void GetGoing(bool isPackaged);
    }


    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("91b1a95b-bc74-4d56-8b9a-032486b18a69")]
    public interface INativeCallback
    {
        void ExitHostApp();
    }


    [Guid("9ee8915b-53c5-4ee6-9148-201f790eac1a")]
    public class Broker : IBroker
    {
        private App _app;
        private CoreDispatcher dispatcher;

        // Never called!
        public Broker()
        {
        }

        public static INativeCallback Callback;

        public void Initialize(INativeCallback callback)
        {
            try
            {
                Callback = callback;
                App.IsIslandLaunch = true;
                _app = new App();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to create App2 with " + ex.Message);
            }
        }

        // Called first to set things up
        public void GetGoing(bool isPackaged)
        {
            var context = SynchronizationContext.Current;
            if (context == null)
            {
                var coreWindow = CoreWindow.GetForCurrentThread();
                if (coreWindow != null)
                {
                    dispatcher = coreWindow.Dispatcher;

                    if (dispatcher != null)
                    {
                        CustomSyncContext sncCtxt = new CustomSyncContext(dispatcher);
                        SynchronizationContext.SetSynchronizationContext(sncCtxt);
                    }
                    else
                    {
                        Debug.WriteLine("FATAL:: No dispatcher configured! Exiting");
                        throw new Exception("FATAL:: No dispatcher configured! Exiting");
                    }
                }
            }
        }

        public UIElement GetUI()
        {
            App.Dispatcher = dispatcher; // Required for any UI thread switching, like property change
            var returnValue = new MainPage();

            return returnValue;
        }
    }
}
