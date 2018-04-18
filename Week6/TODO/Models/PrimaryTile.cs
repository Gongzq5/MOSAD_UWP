using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TODO.Models
{
    public class PrimaryTile
    {
        public string time { get; set; } = "8:15 AM, Saturday";
        public string message { get; set; } = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore.";
        public string message2 { get; set; } = " At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident.";
        public string branding { get; set; } = "name";
        public string appName { get; set; } = "TODO";
        public string imageurl { get; set; } = "";

        public PrimaryTile(string _time, string _title, string _description, string _imageurl)
        {
            time = _time;
            message = _title;
            message2 = _description;
            imageurl = string.IsNullOrEmpty(_imageurl) ? "Assets/icons8-task-50.png" : "ms-appdata:///local/" + _imageurl;            
        }
    }
}
