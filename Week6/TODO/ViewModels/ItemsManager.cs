using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TODO.Models;
using TODO.Services;
using Windows.UI.Notifications;

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

        private ObservableCollection<Models.TodoItem> _collection = new ObservableCollection<TodoItem>();
        public ObservableCollection<Models.TodoItem> Collection
        {
            get { return _collection; }
            set
            {
                _collection = value;
                NotifyPropertyChanged();
                TileService.UpdatePrimaryTile(); 
            }
        }

        private ItemsManager()
        {
            _collection.Clear();
            var sql = "SELECT * FROM ITEMS";
            var conn = new SQLiteConnection("demo.db");
            using (var statement = conn.Prepare(sql))
            {
                while (SQLiteResult.ROW == statement.Step())
                {
                    long id = (long)statement[0];
                    string title = statement[1].ToString();

                    bool? iscomplete = Boolean.Parse(statement[2] as string);

                    var description = statement[3].ToString();

                    var datestr = statement[4].ToString();
                    var datecom = datestr.Substring(0, datestr.IndexOf(" ")).Split('/');
                    DateTimeOffset duedate = (DateTimeOffset)new DateTime(int.Parse(datecom[0]), int.Parse(datecom[1]), int.Parse(datecom[2]));

                    string imgname = statement[5] as string;

                    _collection.Add(new TodoItem(title, description, duedate, iscomplete, imgname)
                    {
                        ID = id
                    });
                }
            }
        }

        public void AddNewItem(Models.TodoItem item)
        {
            DataBaseAccess.InsertToDB(item);
            SyncFromDB();
            TileService.UpdatePrimaryTile();
        }

        public void UpdateItem(long id, string newTitle, string newDescription, DateTimeOffset newDueDate, string newImageUrl)
        {
            foreach (var item in Collection)
            {
                if (item.ID == id)
                {
                    item.Title = newTitle;
                    item.Description = newDescription;
                    item.DueDate = newDueDate;
                    item.ImageUrl = newImageUrl;
                    DataBaseAccess.UpdateItemInDB(id, newTitle, item.IsComplete, newDescription, newDueDate.ToString(), newImageUrl);
                    break;
                }
            }
            TileService.UpdatePrimaryTile();
        }

        public void RemoveItem(long id)
        {
            int WaitingRemoveIndex = 0;
            for (WaitingRemoveIndex=0; WaitingRemoveIndex<Collection.Count; WaitingRemoveIndex++)
            {
                if (Collection[WaitingRemoveIndex].ID == id)
                {
                    Collection.RemoveAt(WaitingRemoveIndex);
                    break;
                }
            }
            DataBaseAccess.RemoveItemInDB(id);
            TileService.UpdatePrimaryTile();
        }
        
        public void SyncFromDB()
        {
            Collection.Clear();
            var sql = "SELECT * FROM ITEMS";
            var conn = new SQLiteConnection("demo.db");
            using (var statement = conn.Prepare(sql))
            {
                while (SQLiteResult.ROW == statement.Step())
                {
                    long id = (long)statement[0];
                    string title = statement[1].ToString();

                    bool? iscomplete = Boolean.Parse(statement[2] as string);

                    var description = statement[3].ToString();

                    var datestr = statement[4].ToString();
                    var datecom = datestr.Substring(0, datestr.IndexOf(" ")).Split('/');
                    DateTimeOffset duedate = (DateTimeOffset)new DateTime(int.Parse(datecom[0]), int.Parse(datecom[1]), int.Parse(datecom[2]));

                    string imgname = statement[5] as string;

                    Collection.Add(new TodoItem(title, description, duedate, iscomplete, imgname)
                    {
                        ID = id
                    });
                }
            }
        }

        public bool IsExsisting(TodoItem newitem)
        {
            foreach (var item in Collection)
            {
                if (item.Title == newitem.Title &&
                    item.Description == newitem.Description &&
                    item.DueDate.Date == newitem.DueDate.Date)
                {
                    return true;
                }
            }
            return false;
        }

        
    }
}
