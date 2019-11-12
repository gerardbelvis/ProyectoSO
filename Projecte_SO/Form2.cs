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
using System.Threading;
using System.Media;

namespace Projecte_SO
{
    public partial class Form2 : Form
    {
        Socket server;
        
        public Form2()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if ((usuari.Text == "") || (contra.Text == "") || (repetircontra.Text == ""))
                MessageBox.Show("Error. Falta algún campo por poner");

            else
            {
                IPAddress direc = IPAddress.Parse("192.168.56.102");
                IPEndPoint ipep = new IPEndPoint(direc, 9070);

                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);

                    string mensaje = "5/" + usuari.Text + "/" + contra.Text + "/" + repetircontra.Text;

                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    server.Send(msg);

                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    if (mensaje == "0")
                        MessageBox.Show("Usuari creat correctament");
                    else
                        MessageBox.Show("Error creant l'usuari");
                    Close();
                }

                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }

                catch (NullReferenceException)
                {
                    MessageBox.Show("Error. No he podido conectar con servidor");
                }
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)

                this.contra.PasswordChar = '\0';
            else
                this.contra.PasswordChar = '*';
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked)

                this.repetircontra.PasswordChar = '\0';
            else
                this.repetircontra.PasswordChar = '*';
        }
    }
}
