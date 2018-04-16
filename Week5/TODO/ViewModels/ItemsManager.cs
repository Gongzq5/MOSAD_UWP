using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TODO.ViewModels
{
    public class ItemsManager : ViewModelBase
    {
        private static ItemsManager instance = null;
        public static ItemsManager Instance
        {
            get
            {
                if (instance == null) instance = new ItemsManager();
                return instance;
            }
        }

        private ItemsManager()
        {
            AddNewItem(new Models.TodoItem("Hello, UWP", "Never learn UWP thanks.", DateTimeOffset.Now, true));
            AddNewItem(new Models.TodoItem("Hello, GZQ", "Hahahaha you are so handsome.", DateTimeOffset.Now));
        }

        public ObservableCollection<Models.TodoItem> Collection { get; } = new ObservableCollection<Models.TodoItem>();

        public void AddNewItem(Models.TodoItem item)
        {
            Collection.Add(item);
        }

        public void UpdateItem(int oldIndex, string newTitle, string newDescription, DateTimeOffset newDueDate, string newImageUrl)
        {
            Collection[oldIndex].Title = newTitle;
            Collection[oldIndex].Description = newDescription;
            Collection[oldIndex].DueDate = newDueDate;
            Collection[oldIndex].ImageUrl = newImageUrl;
        }

        public void RemoveItem(int index)
        {
            Collection.RemoveAt(index);
        }

        public void StoreInDB()
        {
            using (var conn = new SQLiteConnection("demo.db"))
            {

            }
            

        }
    }
}
