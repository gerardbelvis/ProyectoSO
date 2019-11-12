using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace Projecte_SO
{
    public partial class Form1 : Form
    {
        Socket server;
        List<string> conectados = new List<string>();

        public Form1()
        {
            InitializeComponent();
            label3.Visible = false;
            label4.Visible = false;
            label5.Visible = false;
            radioButton1.Visible = false;
            radioButton2.Visible = false;
            radioButton3.Visible = false;
            comboBox1.Visible = false;
            comboBox2.Visible = false;
            comboBox3.Visible = false;
            comboBox4.Visible = false;
            button2.Visible = false;
            label7.Visible = false;
            dameConButton.Visible = false;
            conectadosGrid.Visible = false;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                if (radioButton1.Checked)
                {
                    string missatge = "1/";
                    // Enviamos al servidor 
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(missatge);
                    server.Send(msg);

                    //Rebem la resposta del servidor
                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    missatge = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show("La durada de la partida es " + missatge);
                }

                if (radioButton2.Checked)
                {
                    string missatge = "2/" + comboBox1.Text + "/" + comboBox2.Text;
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(missatge);
                    server.Send(msg);

                    //Rebem la resposta del servidor
                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    missatge = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show("Ha mort  " + missatge + " cops");
                }

                if (radioButton3.Checked)
                {
                    string missatge = "3/" + comboBox3.Text + "/" + comboBox4.Text;
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(missatge);
                    server.Send(msg);

                    //Rebem la resposta del servidor
                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    missatge = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show("El personatge s'ha escollit " + missatge + " cops");
                }
            }

            catch (SocketException)
            {
                MessageBox.Show("Error. No hi ha connexió");
                return;
            }

            catch (NullReferenceException)
            {
                MessageBox.Show("Error en les dades");
            }


        }

        private void button1_Click(object sender, EventArgs e)
        {
            if ((textBox1.Text) == "" || (textBox2.Text) == "")
                MessageBox.Show("Emplena els camps de usuari i contraseña !!");
            else
            {
                IPAddress direc = IPAddress.Parse("192.168.56.102");
                IPEndPoint ipep = new IPEndPoint(direc, 9070);
                //creem socket
                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                   
                    server.Connect(ipep);//Intentem connectar el socket
                    this.BackColor = Color.Green;
                    label3.Visible = true;
                    label4.Visible = true;
                    label5.Visible = true;
                    radioButton1.Visible = true;
                    radioButton2.Visible = true;
                    radioButton3.Visible = true;
                    comboBox1.Visible = true;
                    comboBox2.Visible = true;
                    comboBox3.Visible = true;
                    comboBox4.Visible = true;
                    button2.Visible = true;
                    label7.Visible = true;
                    dameConButton.Visible = true;
                    conectadosGrid.Visible = true;
                    
                    //Añadir a lista de conectados
                    string missatge = "6/" + textBox1.Text;
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(missatge);
                    server.Send(msg);

                    //Rebem la resposta del servidor
                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    missatge = Encoding.ASCII.GetString(msg2).Split('\0')[0];

                    int i = 2;
                    string[] trozosMissatge = missatge.Split(',');

                    if (trozosMissatge[0] == "1")
                    {
                        
                        while (i < trozosMissatge.Length)
                        {
                            conectados.Add(trozosMissatge[i]);
                            i = i + 2;
                        }
                    }   
                    else
                        MessageBox.Show("La llista esta completa");
                }
                catch (SocketException)
                {
                    //Si hi ha exception imprimir l'error i sortim amb el return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
            }
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Form2 register = new Form2();
            register.ShowDialog();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            //Mensaje de desconexión
            string mensaje = "0/";

            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //Nos desconectamos
            this.BackColor = Color.Gray;
            label3.Visible = false;
            label4.Visible = false;
            label5.Visible = false;
            radioButton1.Visible = false;
            radioButton2.Visible = false;
            radioButton3.Visible = false;
            comboBox1.Visible = false;
            comboBox2.Visible = false;
            comboBox3.Visible = false;
            comboBox4.Visible = false;
            button2.Visible = false;
            label7.Visible = false;
            dameConButton.Visible = false;
            conectadosGrid.Visible = false;
            server.Shutdown(SocketShutdown.Both);
            server.Close();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)

                this.textBox2.PasswordChar = '\0';
            else
                this.textBox2.PasswordChar = '*';
        }

        private void dameConButton_Click(object sender, EventArgs e)
        {
            conectadosGrid.ColumnCount = 1;
            conectadosGrid.RowCount = conectados.Count;
            for (int i = 0; i < conectados.Count; i++)
                conectadosGrid.Rows[i].Cells[0].Value = conectados[i];
        }
    }
}

