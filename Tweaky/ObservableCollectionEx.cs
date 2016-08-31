using System.Collections.Generic;
using System.Linq;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Tweaky
{
    public class ObservableCollectionEx<T> : ObservableCollection<T>
    {
        public virtual void NotifyCollectionChanged()
        {
            this.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        protected bool preventCollectionChangedEvents = false;
		protected override void OnCollectionChanged(System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
		{
            if (!preventCollectionChangedEvents)
                base.OnCollectionChanged(e);
		}

		public void AddRange(IEnumerable<T> range)
		{
			foreach (var item in range)
				base.Add(item);
		}

        public void Replace(IEnumerable<T> range, bool useResetEvent = true)
        {
            preventCollectionChangedEvents = true;

            List<T> oldItems = null;
            List<T> newItems = null;

            if (!useResetEvent)
            {
                oldItems = this.Except(range).ToList();
                newItems = range.Except(this).ToList();
            }

            base.Clear();
            foreach (var item in range)
                base.Add(item);

            preventCollectionChangedEvents = false;

            if (useResetEvent)
            {
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
            else
            {
                foreach (var oldItem in oldItems)
                    OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, oldItem));
                foreach (var newItem in newItems)
                    OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, newItem));
            }
        }

        public void RemoveAll(IEnumerable<T> range)
        {
            foreach (var item in range)
                base.Remove(item);
        }
    }
}
