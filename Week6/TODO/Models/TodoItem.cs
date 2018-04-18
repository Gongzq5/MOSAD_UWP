using SQLitePCL;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;


namespace TODO.Models
{
    public class TodoItem : INotifyPropertyChanged
    {

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string propertyname)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyname));
            //DataBaseAccess.UpdateInDB();
        }

        private long _id;
        public long ID
        {
            get { return _id; }
            set { _id = value; }
        }

        private string _title;
        public string Title
        {
            get { return _title; }
            set
            {
                _title = value;
                NotifyPropertyChanged("Title");
            }
        }

        private string _description;
        public string Description
        {
            get { return _description; }
            set
            {
                _description = value;
                NotifyPropertyChanged("Description");
            }
        }

        private DateTimeOffset _duedate;
        public DateTimeOffset DueDate
        {
            get { return _duedate; }
            set
            {
                _duedate = value;
                NotifyPropertyChanged("DueDate");
            }
        }

        private bool? _iscomplete;
        public bool? IsComplete
        {
            get { return _iscomplete; }
            set
            {
                _iscomplete = value;
                using (var conn = new SQLiteConnection("demo.db"))
                {
                    string sql = "UPDATE ITEMS SET COMPLETE = ? WHERE ID = ?";
                    using (var statement = conn.Prepare(sql))
                    {
                        statement.Bind(1, this._iscomplete == null ? "False": this._iscomplete.ToString());
                        statement.Bind(2, this.ID);
                        statement.Step();
                    }
                } 
                NotifyPropertyChanged("IsComplete");
            }
        }

        private string _imageurl;
        public string ImageUrl
        {
            get { return _imageurl; }
            set
            {
                _imageurl = value;
                SetPic();
                NotifyPropertyChanged("ImageUrl");
            }
        }

        private ImageSource _image;
        public ImageSource Image
        {
            get { return _image; }
            set
            {
                _image = value;
                NotifyPropertyChanged("Image");
            }
        }

        public TodoItem(string title, string description, DateTimeOffset duedate, bool? iscomplete = false, string imageurl = "")
        {
            Title = title;
            Description = description;
            DueDate = duedate;
            IsComplete = iscomplete;
            ImageUrl = imageurl;
        }

        public async void SetPic()
        {
            if (string.IsNullOrEmpty(ImageUrl))
            {
                Image = new BitmapImage(new Uri("ms-appx:///Assets/icons8-task-50.png", UriKind.Absolute));
            }
            else
            {
                var file = await ApplicationData.Current.LocalFolder.GetFileAsync(ImageUrl);
                IRandomAccessStream fileStream = await file.OpenAsync(FileAccessMode.Read);
                BitmapImage bitmapImage = new BitmapImage();
                await bitmapImage.SetSourceAsync(fileStream);
                Image = bitmapImage;
            }
        }
    }
}
