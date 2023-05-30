using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace FileEncrypter.Code
{
	public partial class ProjectSelector : Form
	{
		private List<Project> m_oProjects = null;

		public ProjectSelector()
		{
			InitializeComponent();

			TextBox_ProjectName.Enabled = false;
			TextBox_ProjectPath.Enabled = false;
			Button_ProjectPath.Enabled = false;
			Button_OK.Enabled = false;
			Button_Delete.Enabled = false;

			FormBorderStyle = FormBorderStyle.FixedSingle;

			MaximizeBox = false;
			MinimizeBox = false;

			string sProjectsFile = Program.s_sSettingsFolder + "Projects";

			if( File.Exists( sProjectsFile ) )
			{
				try
				{
					Debug.WriteLine( $"Reading projects file at \"{sProjectsFile}\"..." );

					using( StreamReader oProjectsFile = new( sProjectsFile ) )
					{
						bool bReading = true;
						m_oProjects = new List<Project>();

						ListView_Projects.BeginUpdate();
						while( bReading )
						{
							string sName = oProjectsFile.ReadLine();
							string sDataPath = oProjectsFile.ReadLine();

							if( sName != null && sDataPath != null )
							{
								Debug.WriteLine( $"{sName}: {sDataPath}" );

								m_oProjects.Add( new Project() { m_sName = sName, m_sDataPath = sDataPath, m_sPackedDataPath = sDataPath + "\\Packed" } );

								string[] row = new string[] { sName, sDataPath };
								ListView_Projects.Items.Add( new ListViewItem( row ) );
							}
							else
								bReading = false;
						}
						ListView_Projects.EndUpdate();
					}
				}
				catch
				{
					Debug.WriteLine( "Failed to open projects file." );
				}

				if( m_oProjects.Count == 0 )
				{
					m_oProjects = null;
				}
			}
		}

		private void SaveProjects()
		{
			try
			{
				string sProjectsFile = Program.s_sSettingsFolder + "Projects";

				using( StreamWriter oProjectsFile = new( sProjectsFile ) )
				{
					foreach( Project oProject in m_oProjects )
					{
						if( oProject.IsValid() == false )
							continue;
						
						oProjectsFile.WriteLine( oProject.m_sName );
						oProjectsFile.WriteLine( oProject.m_sDataPath );
					}
				}
			}
			catch( Exception ex )
			{
				Debug.WriteLine( $"Failed to save projects file. {ex}" );
			}
		}

		private void Button_New_Click( object sender, EventArgs e )
		{
			if( m_oProjects == null )
				m_oProjects = new List<Project>();

			m_oProjects.Add( new Project() { m_sName = "New Project", m_sDataPath = "", m_sPackedDataPath = "" } );

			ListView_Projects.BeginUpdate();
			string[] row = new string[] { "New Project", "" };
			ListView_Projects.Items.Add( new ListViewItem( row ) );
			ListView_Projects.EndUpdate();

			ListView_Projects.Items[ ListView_Projects.Items.Count - 1 ].Selected = true;
			ListView_Projects.Select();
		}

		private void ListView_Projects_SelectedIndexChanged( object sender, EventArgs e )
		{
			if( ListView_Projects.SelectedItems.Count == 0 )
			{
				TextBox_ProjectName.Enabled = false;
				TextBox_ProjectPath.Enabled = false;
				Button_ProjectPath.Enabled = false;
				Button_OK.Enabled = false;
				Button_Delete.Enabled = false;
				TextBox_ProjectName.Text = "";
				TextBox_ProjectPath.Text = "";
				return;
			}

			TextBox_ProjectName.Enabled = true;
			TextBox_ProjectPath.Enabled = true;
			Button_ProjectPath.Enabled = true;
			Button_Delete.Enabled = true;

			Project oSelectedProject = m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ];

			TextBox_ProjectName.Text = oSelectedProject.m_sName;
			TextBox_ProjectPath.Text = oSelectedProject.m_sDataPath;

			Button_OK.Enabled = oSelectedProject.IsValid();
		}

		private void ChangeListViewItemText( int _iIndex, string _sProjectName, string _sProjectPath )
		{
			if( _iIndex >= ListView_Projects.Items.Count )
				return;

			ListView_Projects.Items[ListView_Projects.SelectedIndices[0]].Text = _sProjectName;
			ListView_Projects.Items[ListView_Projects.SelectedIndices[0]].SubItems[1].Text = _sProjectPath;
		}

		private void TextBox_ProjectName_TextChanged( object sender, EventArgs e )
		{
			if( ListView_Projects.SelectedItems.Count == 0 )
				return;

			Project oSelectedProject = m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ];
			oSelectedProject.m_sName = TextBox_ProjectName.Text;
			m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ] = oSelectedProject;

			ChangeListViewItemText( ListView_Projects.SelectedIndices[0], TextBox_ProjectName.Text, TextBox_ProjectPath.Text );

			Button_OK.Enabled = oSelectedProject.IsValid();
		}

		private void TextBox_ProjectPath_TextChanged( object sender, EventArgs e )
		{
			if( ListView_Projects.SelectedItems.Count == 0 )
				return;

			Project oSelectedProject = m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ];
			oSelectedProject.m_sDataPath = TextBox_ProjectPath.Text;
			oSelectedProject.m_sPackedDataPath = oSelectedProject.m_sDataPath + "\\Packed";
			m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ] = oSelectedProject;
			
			ChangeListViewItemText( ListView_Projects.SelectedIndices[0], TextBox_ProjectName.Text, TextBox_ProjectPath.Text );

			Button_OK.Enabled = oSelectedProject.IsValid();
		}

		private void ValidateCurrentSelection()
		{
			Project oSelectedProject = m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ];

			if( oSelectedProject.IsValid() == false )
			{
				Debug.WriteLine( $"Can't open encrypter for invalid project: \"{oSelectedProject.m_sName}\" at \"{oSelectedProject.m_sDataPath}\"." );
				MainWindow.m_oSelectedProject = new Project();
				return;
			}

			MainWindow.m_oSelectedProject = oSelectedProject;
			this.DialogResult = DialogResult.OK;
			this.Close();
			Debug.WriteLine( $"Opening encrypter for project \"{oSelectedProject.m_sName}\" at \"{oSelectedProject.m_sDataPath}\"." );
		}

		private void ListView_Projects_DoubleClick( object sender, EventArgs e )
		{
			ValidateCurrentSelection();
		}

		private void Button_OK_Click( object sender, EventArgs e )
		{
			ValidateCurrentSelection();
		}

		private void Button_ProjectPath_Click( object sender, EventArgs e )
		{
			System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
			dialog.SelectedPath = TextBox_ProjectPath.Text;
			if( dialog.ShowDialog() == DialogResult.OK )
			{
				TextBox_ProjectPath.Text = dialog.SelectedPath;

				Project oSelectedProject = m_oProjects[ListView_Projects.SelectedIndices[0]];
				oSelectedProject.m_sDataPath = dialog.SelectedPath;
				oSelectedProject.m_sPackedDataPath = oSelectedProject.m_sDataPath + "\\Packed";
				m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ] = oSelectedProject;

				if( oSelectedProject.IsValid() )
					SaveProjects();
			}

			TextBox_ProjectPath_TextChanged( sender, e );
		}

		private void TextBox_ProjectPath_Validated( object sender, EventArgs e )
		{
			Project oSelectedProject = m_oProjects[ ListView_Projects.SelectedIndices[ 0 ] ];
			if( oSelectedProject.IsValid() )
				SaveProjects();
		}

		private void TextBox_ProjectPath_KeyUp( object sender, KeyEventArgs e )
		{
			if( e.KeyCode == Keys.Enter )
			{
				Project oSelectedProject = m_oProjects[ListView_Projects.SelectedIndices[0]];
				if( oSelectedProject.IsValid() )
					SaveProjects();

				e.Handled = true;
			}
		}

		private void Button_Delete_Click( object sender, EventArgs e )
		{
			m_oProjects.RemoveAt( ListView_Projects.SelectedIndices[0] );
			ListView_Projects.Items.RemoveAt( ListView_Projects.SelectedIndices[0] );

			SaveProjects();
		}
	}
}
