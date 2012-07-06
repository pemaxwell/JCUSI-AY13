using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MySql.Data.MySqlClient;
using System.Threading;
using command_UI;
using MyLibVLC;

namespace Command_Center_3
{
    public partial class Main_Window : Form
    {
        public string server = "localhost";
        public string schema = "commandcenter_db";
        public string user = "root";
        public string password = "abc";
        //MySqlConnection mySqlConnection = new MySqlConnection("SERVER="+server+";DATABASE="+schema+";UID="+user+";PASSWORD="+password+";");
        MySqlDataAdapter robotsSqlDataAdapter;
        MySqlCommandBuilder robotsSqlCommandBuilder;
        DataTable robotsDataTable;
        BindingSource robotsBindingSource;
        string robotsQuery;

        MySqlDataAdapter targetsSqlDataAdapter;
        MySqlCommandBuilder targetsSqlCommandBuilder;
        DataTable targetsDataTable;
        BindingSource targetsBindingSource;
        string targetsQuery;

        string selectedSerial = "";
        string selectedRobotType = "";
        string selectedLatitude = "";
        string selectedLongitude = "";
        string selectedElevation = "";
        string selectedBearing = "";
        string selectedSpeed = "";
        string selectedMission = "";

        Thread updateThread;
        private Object thislock = new Object();

        //VlcInstance instance;
        //VlcMediaPlayer player;
        //string currentVideoAddr = "";
        StaticMap map;

        public Main_Window()
        {
            InitializeComponent();   
        }

        public void UpdateView()
        {
            RefreshMap();
                if (dataGridView2.SelectedRows.Count > 0)
                {
                        string targetsRowID = dataGridView2.SelectedRows[0].Cells[0].Value.ToString();
                        targetsDataTable.Clear();
                        targetsSqlDataAdapter.Fill(targetsDataTable);
                        dataGridView2.ClearSelection();

                        int i = 0;
                        while (i < dataGridView2.Rows.Count)
                        { 
                                Object row = dataGridView2.Rows[i].Cells[0].Value;
                                Object nullObject = null;
                                if (row == nullObject)
                                {
                                    
                                }
                                else
                                {
                                    bool b = (targetsRowID == row.ToString());
                                    if (b)
                                    {
                                        dataGridView2.Rows[i].Selected = true;
                                    }
                                    i++;
                                }
                        }
                    
                }
                else
                {
                    targetsDataTable.Clear();
                    targetsSqlDataAdapter.Fill(targetsDataTable);
                    dataGridView2.ClearSelection();
                }
 
                if (dataGridView1.SelectedRows.Count > 0)
                {
                    //update robots grid and reset highlighted row
                    string robotsRowID = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                    robotsDataTable.Clear();
                    robotsSqlDataAdapter.Fill(robotsDataTable);

                        int j = 0;
                        while (j < dataGridView1.Rows.Count)
                        {
                                Object row = dataGridView1.Rows[j].Cells[0].Value;
                                Object nullObject = null;
                                if (row == nullObject)
                                {

                                }
                                else
                                {
                                    bool b = (robotsRowID == row.ToString());
                                    if (b)
                                    {
                                        dataGridView1.Rows[j].Selected = true;
                                    }
                                    j++;
                                }
                        }
                }
                else
                {
                    robotsDataTable.Clear();
                    robotsSqlDataAdapter.Fill(robotsDataTable);
                    dataGridView1.ClearSelection();
                }
        }

        public void Update_Tables_Thread()
        {
            while (true)
            {
                this.UpdateView();
                Thread.Sleep(5000);
            }
            
        }

        private void Add_Target_Button_Click(object sender, EventArgs e)
        {
            Form myForm = new Add_Target();
            myForm.Show();
            dataGridView2.Update();
        }

        private void Exit_Button_Click(object sender, EventArgs e)
        {
            
            System.Environment.Exit(0);
        }

        private void Clear_Database_Button_Click(object sender, EventArgs e)
        {
            Form myForm = new Clear_Database_Confirm();
            myForm.Show();
        }

        private void Clear_Target_Button_Click(object sender, EventArgs e)
        {
            Form myForm = new Clear_All_Targets_Confirm();
            myForm.Show();
        }

        private void EndEx_Button_Click(object sender, EventArgs e)
        {
            Form myForm = new EndEx_Confirm();
            myForm.Show();
        }

        private void Edit_Target_Button_Click(object sender, EventArgs e)
        {
            if (dataGridView2.SelectedRows.Count > 0)
            {
                string selectedTarget = dataGridView2.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Edit_Target(selectedTarget);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a target first.", "Select Target");
            }
        }

        private void Register_Robot_Button_Click(object sender, EventArgs e)
        {
            Form myForm = new Register_Robot();
            myForm.Show();
        }

        private void Delete_Target_Button_Click(object sender, EventArgs e)
        {
            if (dataGridView2.SelectedRows.Count > 0)
            {
                string tID = dataGridView2.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Delete_Target(tID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a target first.","Select Target");
            } 
        }

        private void Edit_Robot_Button_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                string rID = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Edit_Robots(rID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a robot first.", "Select Robot");
            }
        }

        private void Delete_Robot_Button_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                string rID = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Delete_Robot(rID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a robot first.", "Select Robot");
            }
        }

        private void DataGridView1_DataError(object sender, DataGridViewDataErrorEventArgs anError)
        {
            //MessageBox.Show("Error happened " + anError.Context.ToString());
        }

        private void DataGridView2_DataError(object sender, DataGridViewDataErrorEventArgs anError)
        {
            //MessageBox.Show("Error happened " + anError.Context.ToString());
        }

        private void RefreshMap()
        {
            map.Markers.Clear();

            string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            MySqlDataReader Reader;
            connection.Open();
            command.CommandText = "SELECT * FROM (SELECT r.serialNum as Serial_Number,rtl.type as Robot_Type,rs.latitude AS Latitude,rs.longitude AS Longitude,rs.elevation as Elevation,rs.bearing as Bearing,rs.speed as Speed,mlu.missionDescription as Current_Mission FROM robots AS r INNER JOIN robotstate AS rs ON r.serialNum = rs.serialNum INNER JOIN missionlookup AS mlu ON r.currentMission = mlu.missionID INNER JOIN robottypelookup AS rtl ON r.robotType = rtl.typeID ORDER BY rs.timeStamp DESC) as inter_view GROUP BY Serial_number";
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                StaticMap.Marker m = new StaticMap.Marker();
                m.Lat = (double)Reader.GetValue(2);
                m.Lng = (double)Reader.GetValue(3);
                if (Reader.GetValue(1).ToString() == "LandBot")
                {
                    m.Color = (StaticMap.mColor)2; // Green
                }
                else if (Reader.GetValue(1).ToString() == "AirBot")
                {
                    m.Color = (StaticMap.mColor)5; // Blue
                }
                else if (Reader.GetValue(1).ToString() == "SeaBot")
                {
                    m.Color = (StaticMap.mColor)4; // Yellow
                }
                if (dataGridView1.SelectedRows.Count > 0)
                {
                    if (Reader.GetValue(0).ToString() == selectedSerial)
                    {
                        m.Size = (StaticMap.mSize)1; // bigger marker size
                    }
                    else
                    {
                        m.Size = (StaticMap.mSize)2; // marker size
                    }
                }
                else
                {
                    m.Size = (StaticMap.mSize)2;
                }
                map.Markers.Add(m);
            }
            Reader.Close();

            command.CommandText = "SELECT * FROM (SELECT t.targetID AS ID, tl.latitude AS Latitude, tl.longitude AS Longitude, tl.timeUpdated AS Time_Identified, t.taskedRobot AS Tasked_Robot FROM targets as t LEFT JOIN targetlocations as tl ON t.targetID = tl.targetID ORDER BY Time_Identified DESC) AS inter_view GROUP BY ID";
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                StaticMap.Marker m = new StaticMap.Marker();
                string lat = Reader.GetValue(1).ToString();
                string lng = Reader.GetValue(2).ToString();
                if (lat.ToString() == "")
                {
                    m.Lat = 0.0;
                    m.Lng = 0.0;
                }
                else
                {
                    m.Lat = Double.Parse(lat);
                    m.Lng = Double.Parse(lng);
                }
                m.Color = (StaticMap.mColor)8; // Red
                m.Size = (StaticMap.mSize)2; // marker size
                map.Markers.Add(m);
            }

            connection.Close();

            webBrowser1.Navigate(map.Render());
        }

        private void refreshMapButton_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                this.selectedSerial = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
            }
            RefreshMap();
        }

        /*private void changeVideo(string addr)
        {
            player.Stop();
            using (VlcMedia media = new VlcMedia(instance, "udp://@"+addr))
            {
                if (player == null)
                    player = new VlcMediaPlayer(media);
                else
                    player.Media = media;
            }
            player.Drawable = pictureBox1.Handle;
            player.Play();
        }*/

        private void View_Info_Button_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                this.selectedSerial = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                RefreshMap();
                string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
                MySqlConnection connection = new MySqlConnection(MyConString);
                MySqlCommand command = connection.CreateCommand();
                MySqlDataReader Reader;
                connection.Open();
                command.CommandText = "SELECT ipaddr FROM robots WHERE serialNum = " + selectedSerial;
                Reader = command.ExecuteReader();
                /*while (Reader.Read())
                {
                    this.currentVideoAddr = Reader.GetValue(0).ToString();
                }*/
                connection.Close();

                //changeVideo(currentVideoAddr);

                selectedRobotType = dataGridView1.SelectedRows[0].Cells[1].Value.ToString();
                selectedLatitude = dataGridView1.SelectedRows[0].Cells[2].Value.ToString();
                selectedLongitude = dataGridView1.SelectedRows[0].Cells[3].Value.ToString();
                selectedElevation = dataGridView1.SelectedRows[0].Cells[4].Value.ToString();
                selectedBearing = dataGridView1.SelectedRows[0].Cells[5].Value.ToString();
                selectedSpeed = dataGridView1.SelectedRows[0].Cells[6].Value.ToString();
                selectedMission = dataGridView1.SelectedRows[0].Cells[7].Value.ToString();

                serialNum.Text = selectedSerial;
                robotType.Text = selectedRobotType;
                latitude.Text = selectedLatitude + " degrees";
                longitude.Text = selectedLongitude + " degrees";
                elevation.Text = selectedElevation + " meters";
                bearing.Text = selectedBearing + " degrees";
                speed.Text = selectedSpeed + " meters/second";
                mission.Text = selectedMission + "...";
            }
            
        }

        private void Main_Window_Load(object sender, EventArgs e)
        {
            string[] args = new string[] {
                "-I", "dummy", "--ignore-config",
                @"--plugin-path=C:\Program Files\VideoLAN\VLC\plugins"
                //,"--vout-filter=deinterlace", "--deinterlace-mode=blend"
            };
            //instance = new VlcInstance(args);
            //player = null;

            MySqlConnection mySqlConnection = new MySqlConnection("SERVER=" + server + ";DATABASE=" + schema + ";UID=" + user + ";PASSWORD=" + password + ";");
            //robots table
            robotsQuery = "SELECT * FROM (SELECT r.serialNum as Serial_Number,rtl.type as Robot_Type,rs.latitude AS Latitude,rs.longitude AS Longitude,rs.elevation as Elevation,rs.bearing as Bearing,rs.speed as Speed,mlu.missionDescription as Current_Mission FROM robots AS r LEFT JOIN robotstate AS rs ON r.serialNum = rs.serialNum INNER JOIN missionlookup AS mlu ON r.currentMission = mlu.missionID INNER JOIN robottypelookup AS rtl ON r.robotType = rtl.typeID ORDER BY rs.timeStamp DESC) as inter_view GROUP BY Serial_number";
            robotsSqlDataAdapter = new MySqlDataAdapter(robotsQuery, mySqlConnection);
            robotsSqlCommandBuilder = new MySqlCommandBuilder(robotsSqlDataAdapter);
            robotsDataTable = new DataTable();
            robotsSqlDataAdapter.Fill(robotsDataTable);
            robotsBindingSource = new BindingSource();
            robotsBindingSource.DataSource = robotsDataTable;
            dataGridView1.DataSource = robotsBindingSource;
            dataGridView1.ClearSelection();

            //targets table
            targetsQuery = "SELECT * FROM (SELECT t.targetID AS ID, tl.latitude AS Latitude, tl.longitude AS Longitude, tl.timeUpdated AS Time_Identified, t.taskedRobot AS Tasked_Robot FROM targets as t LEFT JOIN targetlocations as tl ON t.targetID = tl.targetID ORDER BY Time_Identified DESC) AS inter_view GROUP BY ID";
            targetsSqlDataAdapter = new MySqlDataAdapter(targetsQuery, mySqlConnection);
            targetsSqlCommandBuilder = new MySqlCommandBuilder(targetsSqlDataAdapter);
            targetsDataTable = new DataTable();
            targetsSqlDataAdapter.Fill(targetsDataTable);
            targetsBindingSource = new BindingSource();
            targetsBindingSource.DataSource = targetsDataTable;
            dataGridView2.DataSource = targetsBindingSource;
            dataGridView2.ClearSelection();

            updateThread = new Thread(Update_Tables_Thread);
            updateThread.Start();
            updateThread.IsBackground = true;

            map = new StaticMap
            {
                Width = 1000,
                Height = 480,
                Zoom = 18,
                LatCenter = Double.Parse("41.39103611"),
                LngCenter = Double.Parse("-73.95241944"),
                Type = StaticMap.mType.Hybrid
            };

            RefreshMap();

            /*using (VlcMedia media = new VlcMedia(instance, "udp://@" + currentVideoAddr))
            {
                if (player == null)
                    player = new VlcMediaPlayer(media);
                else
                    player.Media = media;
            }
            player.Drawable = pictureBox1.Handle;
            player.Play();*/
        }

        private void aboutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Form myForm = new About();
            myForm.Show();
        }

        private void RegisterRobotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form myForm = new Register_Robot();
            myForm.Show();
        }

        private void deleteSelectedRobotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                string rID = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Delete_Robot(rID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a robot first.", "Select Robot");
            }
        }

        private void editTargetToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView2.SelectedRows.Count > 0)
            {
                string selectedTarget = dataGridView2.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Edit_Target(selectedTarget);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a target first.", "Select Target");
            }
        }

        private void deleteSelectedTargetToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView2.SelectedRows.Count > 0)
            {
                string tID = dataGridView2.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Delete_Target(tID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a target first.", "Select Target");
            } 
        }

        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form myForm = new Settings();
            myForm.Show();
        }

        private void RefreshMapToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                this.selectedSerial = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
            }
            RefreshMap();
        }

        private void ViewRobotStripMenuItem1_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                this.selectedSerial = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                RefreshMap();
                string MyConString = "SERVER=" + server + ";DATABASE=" + schema + ";UID=" + user + ";PASSWORD=" + password + ";";
                MySqlConnection connection = new MySqlConnection(MyConString);
                MySqlCommand command = connection.CreateCommand();
                MySqlDataReader Reader;
                connection.Open();
                command.CommandText = "SELECT ipaddr FROM robots WHERE serialNum = " + selectedSerial;
                Reader = command.ExecuteReader();
                /*while (Reader.Read())
                {
                    this.currentVideoAddr = Reader.GetValue(0).ToString();
                }*/
                connection.Close();

                //changeVideo(currentVideoAddr);

                selectedRobotType = dataGridView1.SelectedRows[0].Cells[1].Value.ToString();
                selectedLatitude = dataGridView1.SelectedRows[0].Cells[2].Value.ToString();
                selectedLongitude = dataGridView1.SelectedRows[0].Cells[3].Value.ToString();
                selectedElevation = dataGridView1.SelectedRows[0].Cells[4].Value.ToString();
                selectedBearing = dataGridView1.SelectedRows[0].Cells[5].Value.ToString();
                selectedSpeed = dataGridView1.SelectedRows[0].Cells[6].Value.ToString();
                selectedMission = dataGridView1.SelectedRows[0].Cells[7].Value.ToString();

                serialNum.Text = selectedSerial;
                robotType.Text = selectedRobotType;
                latitude.Text = selectedLatitude + " degrees";
                longitude.Text = selectedLongitude + " degrees";
                elevation.Text = selectedElevation + " meters";
                bearing.Text = selectedBearing + " degrees";
                speed.Text = selectedSpeed + " meters/second";
                mission.Text = selectedMission + "...";
            }
            else
            {
                MessageBox.Show("Please select a robot first.", "Select Robot");
            } 
        }

        private void editSelectedRobotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                string rID = dataGridView1.SelectedRows[0].Cells[0].Value.ToString();
                Form myForm = new Edit_Robots(rID);
                myForm.Show();
            }
            else
            {
                MessageBox.Show("Please select a robot first.", "Select Robot");
            }
        }
    }
}
