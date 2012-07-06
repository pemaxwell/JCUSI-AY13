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
    public partial class EndEx_Confirm : Form
    {
        string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";

        public EndEx_Confirm()
        {
            InitializeComponent();
        }

        private void Cancel_Button_Click_1(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Confirm_Button_Click_1(object sender, EventArgs e)
        {
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();

            connection.Open();
            command.CommandText = "UPDATE missioncomplete SET missionComplete=1";
            command.ExecuteNonQuery();
            connection.Close();
            this.Close();
        }
    }
}
