using System;
using System.Diagnostics.Contracts;
using System.Security;
using System.Threading;
using Windows.UI.Core;

namespace XamlIslandHelloWorld
{
    public class CustomSyncContext : SynchronizationContext
    {
        private readonly CoreDispatcher m_dispatcher;

        internal  CustomSyncContext(CoreDispatcher dispatcher)
        {
            m_dispatcher = dispatcher;
        }

        private class Invoker
        {
            private readonly SendOrPostCallback m_callback;
            private readonly object m_state;

            public static readonly Action<object> InvokeDelegate = Invoke;

            public Invoker(SendOrPostCallback callback, object state)
            {
                m_callback = callback;
                m_state = state;
            }

            public static void Invoke(object thisObj)
            {
                var invoker = (Invoker)thisObj;
                invoker.InvokeCore();
            }

            public void InvokeCore()
            {
                SynchronizationContext prevSyncCtx = SynchronizationContext.Current;
                try
                {
                    m_callback(m_state);
                }
                catch (Exception ex)
                {
                    //
                    // If we let exceptions propagate to CoreDispatcher, it will swallow them with the idea that someone will
                    // observe them later using the IAsyncInfo returned by CoreDispatcher.RunAsync.  However, we ignore
                    // that IAsyncInfo, because there's nothing Post can do with it (since Post returns void).
                    // So, we report these as unhandled exceptions.
                    //
                    throw ex;
//                    RuntimeAugments.ReportUnhandledException(ex);
                }
                finally
                {
                    SynchronizationContext.SetSynchronizationContext(prevSyncCtx);
                }
            }
        }


        [SecuritySafeCritical]
        public override void Post(SendOrPostCallback d, object state)
        {
            if (d == null)
                throw new ArgumentNullException("d");

            Contract.EndContractBlock();

            var invoker = new Invoker(d, state);
            m_dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                Invoker.InvokeDelegate(invoker);
            });
        }

        [SecuritySafeCritical]
        public override void Send(SendOrPostCallback d, object state)
        {
            throw new NotSupportedException("Send not supported");
        }

        public override SynchronizationContext CreateCopy()
        {
            return new CustomSyncContext(m_dispatcher);
        }
    }
}


