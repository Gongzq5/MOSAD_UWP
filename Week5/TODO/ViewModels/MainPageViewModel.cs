using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TODO.ViewModels
{
    public class MainPageViewModel : ViewModelBase
    {
        private static MainPageViewModel instance = null;
        public static MainPageViewModel Instance
        {
            get
            {
                if (instance == null) instance = new MainPageViewModel();
                return instance;
            }
        }

        private MainPageViewModel()
        {
            collection = ItemsManager.Instance.Collection;
        }

        private ObservableCollection<Models.TodoItem> collection;
        public ObservableCollection<Models.TodoItem> Collection
        {
            get
            {
                return collection;
            }
        }


    }
}
