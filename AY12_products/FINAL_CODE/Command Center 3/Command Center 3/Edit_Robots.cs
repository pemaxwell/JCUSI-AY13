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
    public partial class Edit_Robots : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
        private string rID;

        public Edit_Robots(string robotID)
        {
            InitializeComponent();
            rID = robotID;

            string defaultRobotType = "";
            string defaultMission = "";
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            MySqlDataReader Reader;
            command.CommandText = "SELECT * FROM (SELECT r.serialNum as Serial_Number,rtl.type as Robot_Type,rs.latitude AS Latitude,rs.longitude AS Longitude,rs.elevation as Elevation,rs.bearing as Bearing,rs.speed as Speed,mlu.missionDescription as Current_Mission FROM robots AS r LEFT JOIN robotstate AS rs ON r.serialNum = rs.serialNum INNER JOIN missionlookup AS mlu ON r.currentMission = mlu.missionID INNER JOIN robottypelookup AS rtl ON r.robotType = rtl.typeID ORDER BY rs.timeStamp DESC) as inter_view WHERE Serial_Number = "+rID+" GROUP BY Serial_number";
            connection.Open();
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                defaultRobotType = Reader.GetValue(1).ToString();
                //comboBox1.Items.Add(defaultRobotType);
                textBox1.Text = Reader.GetValue(2).ToString();
                textBox2.Text = Reader.GetValue(3).ToString();
                textBox3.Text = Reader.GetValue(4).ToString();
                textBox4.Text = Reader.GetValue(5).ToString();
                textBox5.Text = Reader.GetValue(6).ToString();
                defaultMission = Reader.GetValue(7).ToString();
                //comboBox2.Items.Add(defaultMission);
            }
            Reader.Close();
            
            command.CommandText = "SELECT type FROM robottypelookup";
            Reader = command.ExecuteReader();
            int r = 0;
            while (Reader.Read())
            {
                string thisrow = "";
                for (int i = 0; i < Reader.FieldCount; i++)
                {
                    thisrow += Reader.GetValue(i).ToString();
                    comboBox1.Items.Add(thisrow);
                    if (thisrow == defaultRobotType)
                        comboBox1.SelectedIndex = r;
                }
                r++;
            }
            Reader.Close();

            command.CommandText = "SELECT missionDescription FROM missionlookup";
            Reader = command.ExecuteReader();
            int b = 0;
            while (Reader.Read())
            {
                string thisrow = "";
                for (int i = 0; i < Reader.FieldCount; i++)
                {
                    thisrow += Reader.GetValue(i).ToString();
                    comboBox2.Items.Add(thisrow);
                    if (thisrow == defaultMission)
                        comboBox2.SelectedIndex = b;
                }
                b++;
            }
            connection.Close();

        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
           
            MySqlConnection mySqlConnection = new MySqlConnection("SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;");

            string robotType = comboBox1.SelectedItem.ToString();
            string latitude = textBox1.Text;
            string longitude = textBox2.Text;
            string elevation = textBox3.Text;
            string bearing = textBox4.Text;
            string speed = textBox5.Text;
            string missionType = comboBox2.SelectedItem.ToString();

            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            connection.Open();
            command.CommandText = "UPDATE robots SET robotType = (SELECT typeID FROM robottypelookup WHERE type = '" + robotType + "'), currentMission = (SELECT missionID FROM missionlookup WHERE missionDescription = '" + missionType + "') WHERE serialNum = " + rID;
            
            command.ExecuteNonQuery();
            command.CommandText = "INSERT INTO robotstate (serialNum,latitude,longitude,elevation,bearing,speed,timeStamp) VALUES(" + rID + "," + latitude + "," + longitude + "," + elevation + "," + bearing + "," + speed + ",CONCAT(CURDATE(),' ',CURTIME()))";
            
            command.ExecuteNonQuery();

            connection.Close();
            this.Close();
        }

    }
}
