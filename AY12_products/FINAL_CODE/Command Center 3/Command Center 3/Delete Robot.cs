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
    public partial class Delete_Robot : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
        private string robotID;

        public Delete_Robot(string rID)
        {
            InitializeComponent();
            robotID = rID;
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
            command.CommandText = "UPDATE targets SET taskedRobot = null WHERE taskedRobot = " + this.robotID;
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM robotstate WHERE serialNum = " + this.robotID;
            command.ExecuteNonQuery();
            command.CommandText = "DELETE FROM robots WHERE serialNum = " + this.robotID;
            command.ExecuteNonQuery();
            connection.Close();

            this.Close();
        }
    }
}
