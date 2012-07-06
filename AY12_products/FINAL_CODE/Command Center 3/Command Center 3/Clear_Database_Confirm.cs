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
    public partial class Clear_Database_Confirm : Form
    {
        string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";

        public Clear_Database_Confirm()
        {
            InitializeComponent();
        }

        private void Confirm_Click(object sender, EventArgs e)
        {
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            
            connection.Open();
            command.CommandText = "DELETE FROM robotstate";
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM targetlocations";
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM targets";
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM robots";
            command.ExecuteNonQuery();
            command.CommandText = "UPDATE missioncomplete SET missionComplete=0";
            command.ExecuteNonQuery();
            connection.Close();
            this.Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
