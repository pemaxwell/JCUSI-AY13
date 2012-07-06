using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MySql.Data.MySqlClient;

namespace Command_Center_3
{
    public partial class Delete_Target : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
        private string targetID;

        public Delete_Target(string tID)
        {
            InitializeComponent();
            targetID = tID;
        }

        private void Cancel_Button_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Confirm_Button_Click(object sender, EventArgs e)
        {
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            connection.Open();
            command.CommandText = "DELETE FROM targetlocations WHERE targetID = " + this.targetID; 
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM targets WHERE targetID = " + this.targetID;
            command.ExecuteNonQuery();
            connection.Close();
            this.Close();
        }

    }
}
