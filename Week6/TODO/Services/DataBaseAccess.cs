using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TODO.Models;

namespace TODO.Services
{
    public static class DataBaseAccess
    {
        public static void InitializeDatabase()
        {
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare(
                    "CREATE TABLE IF NOT EXISTS [ITEMS](" +
                    "[ID] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," +
                    "[TITLE] TEXT NOT NULL," +
                    "[COMPLETE] BOOLEAN," +
                    "[DESCRIPTION] TEXT NOT NULL," +
                    "[DUEDATE] TEXT NOT NULL," +
                    "[IMAGEURL] TEXT" +
                    ");"))
                {
                    statement.Step();
                }
            }
        }
        
        public static void InsertToDB(TodoItem item)
        {
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare(
                    "INSERT INTO ITEMS (TITLE, COMPLETE, DESCRIPTION, DUEDATE, IMAGEURL) VALUES (?,?,?,?,?)"))
                {
                    statement.Bind(1, item.Title);
                    statement.Bind(2, item.IsComplete.ToString() ?? "False");
                    statement.Bind(3, item.Description);
                    statement.Bind(4, item.DueDate.Date.ToString());
                    statement.Bind(5, item.ImageUrl);
                    statement.Step();
                }
            }
        }

        public static void UpdateItemInDB(long id, string newtitle, bool? newcompletestate, string newdescription, string newduedate, string newimageurl)
        {
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare("UPDATE ITEMS SET TITLE = ?, COMPLETE = ?, DESCRIPTION = ?, DUEDATE = ?, IMAGEURL = ?  WHERE ID = ?"))
                {
                    statement.Bind(1, newtitle);
                    statement.Bind(2, newcompletestate.ToString());
                    statement.Bind(3, newdescription);
                    statement.Bind(4, newduedate);
                    statement.Bind(5, newimageurl);
                    statement.Bind(6, id);
                    statement.Step();
                }
            }
        }

        public static void RemoveItemInDB(long id)
        {
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare("DELETE FROM ITEMS WHERE ID=? "))
                {
                    statement.Bind(1, id);
                    statement.Step();
                }
            }
        }
    }
}
