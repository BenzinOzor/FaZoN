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
	public partial class MainWindow : Form
	{
		private delegate void SafeCallDelegate( FileEncryptionEvent _eEvent, string _sFile );

		public static Project m_oSelectedProject { get; set; }
		private bool m_bProcessingCheckboxes = false;
		private List<TreeNode> m_oCheckedItems = null;
		private List<String> m_oRawFiles = null;

		public MainWindow()
		{

			InitializeComponent();

			if( m_oSelectedProject.IsValid() == false )
				this.Close();

			this.Text = $"File Encrypter - {m_oSelectedProject.m_sName}";

			Encrypter.InitKeys( m_oSelectedProject.m_sName );

			UpdateTreeView( ref TreeView_RawFiles, m_oSelectedProject.m_sDataPath );
			UpdateTreeView( ref TreeView_EncryptedFiles, m_oSelectedProject.m_sPackedDataPath );
			GetAllFiles( ref m_oRawFiles, TreeView_RawFiles.Nodes );
			Label_TotalAssets.Text = $"{ m_oRawFiles.Count } Assets";

			Button_BuildSelected.Enabled = false;
			Button_ClearSelection.Enabled = false;
			Label_EncryptingCount.Text = "";
		}

		////////////////////////// CLASS FUNCTIONS //////////////////////////

		private void UpdateTreeView( ref TreeView _oTreeView, string _sPath )
		{
			if( Directory.Exists( _sPath ) == false )
				return;

			_oTreeView.BeginUpdate();
			_oTreeView.Nodes.Clear();

			string[] pDirectories = Directory.GetDirectories( _sPath );

			foreach( string sDirectory in pDirectories )
			{
				if( _oTreeView == TreeView_RawFiles && sDirectory.IndexOf( "\\Packed" ) >= 0 )
					continue;

				ProcessDirectory( _oTreeView.Nodes, sDirectory );
			}

			string[] pFiles = Directory.GetFiles( _sPath );

			foreach( string sFile in pFiles )
			{
				_oTreeView.Nodes.Add( Program.GetFileOrDirectoryName( sFile ) );
			}

			_oTreeView.EndUpdate();
		}

		private void ProcessDirectory( TreeNodeCollection _oNode, string _sDirectory )
		{
			_oNode.Add( Program.GetFileOrDirectoryName( _sDirectory ) );

			string[] pDirectories = Directory.GetDirectories( _sDirectory );

			foreach( string sDirectory in pDirectories )
			{
				ProcessDirectory( _oNode[ _oNode.Count - 1 ].Nodes, sDirectory );
			}

			string[] pFiles = Directory.GetFiles( _sDirectory );

			foreach( string sFile in pFiles )
			{
				_oNode[ _oNode.Count - 1 ].Nodes.Add( Program.GetFileOrDirectoryName( sFile ) );
			}
		}

		private void CheckDirectoryChildren( TreeNode _oNode )
		{
			foreach( TreeNode oNode in _oNode.Nodes )
			{
				oNode.Checked = _oNode.Checked;

				if( oNode.Nodes.Count > 0 )
					CheckDirectoryChildren( oNode );
				else
				{
					if( oNode.Checked )
						m_oCheckedItems.Add( oNode );
					else
						m_oCheckedItems.Remove( oNode );
				}
			}
		}

		private List< string > GetCheckedFiles()
		{
			if( m_oCheckedItems.Count <= 0 )
				return null;

			List<string> oFiles = new List<string>();

			foreach( TreeNode oNode in m_oCheckedItems )
				oFiles.Add( GetFileCompletePath( oNode, false ) );

			return oFiles;
		}

		private void GetAllFiles( ref List<string> _oFiles, TreeNodeCollection _oNodes )
		{
			if( _oFiles == null )
				_oFiles = new List<string>();

			bool bPacked = _oNodes == TreeView_EncryptedFiles.Nodes;

			foreach( TreeNode oNode in _oNodes )
			{
				if( oNode.Nodes.Count > 0 )
					GetAllFiles( ref _oFiles, oNode.Nodes );
				else
					_oFiles.Add( GetFileCompletePath( oNode, bPacked ) );
			}
		}

		private void TreeView_RawFiles_AfterCheck( object sender, TreeViewEventArgs e )
		{
			if( m_bProcessingCheckboxes == true )
				return;

			m_bProcessingCheckboxes = true;

			if( m_oCheckedItems == null )
				m_oCheckedItems = new List<TreeNode>();

			if( e.Node.Nodes.Count > 0 )
				CheckDirectoryChildren( e.Node );
			else
			{
				if( e.Node.Checked )
					m_oCheckedItems.Add( e.Node );
				else
					m_oCheckedItems.Remove( e.Node );
			}

			m_bProcessingCheckboxes = false;

			if( m_oCheckedItems.Count > 0 )
			{
				Button_BuildSelected.Enabled = true;
				Button_ClearSelection.Enabled = true;
			}
			else
			{
				Button_BuildSelected.Enabled = false;
				Button_ClearSelection.Enabled = false;
			}
			
			Label_CheckItems.Text = $"{m_oCheckedItems.Count} Selected";
		}

		public void OnFileEncryptionEvent( FileEncryptionEvent _eEvent, string _sFile )
		{
			var d = new SafeCallDelegate( OnFileEncrypted_Safe );
			ProgressBar_Build.Invoke( d, new object[] { _eEvent, _sFile } );
		}

		private void OnFileEncrypted_Safe( FileEncryptionEvent _eEvent, string _sFile )
		{
			switch( _eEvent )
			{
				case FileEncryptionEvent.Encrypting:
					{
						LogFileEvent( $"Encrypting: {_sFile}.", Color.Black );
						break;
					}
				case FileEncryptionEvent.Skipped:
					{
						LogFileEvent( $"Skipped: {_sFile}.", Color.Gray );
						++ProgressBar_Build.Value;
						Label_EncryptingCount.Text = $"{ProgressBar_Build.Value} /{ProgressBar_Build.Maximum}";
						break;
					}
				case FileEncryptionEvent.FileDone:
					{
						++ProgressBar_Build.Value;
						Label_EncryptingCount.Text = $"{ProgressBar_Build.Value} /{ProgressBar_Build.Maximum}";
						break;
					}
				case FileEncryptionEvent.EncryptionDone:
					{
						LogFileEvent( $"Encryption done.", Color.DarkGreen );
						UpdateTreeView( ref TreeView_EncryptedFiles, m_oSelectedProject.m_sPackedDataPath );
						break;
					}
				default:
					break;
			}
		}

		private void LogFileEvent( string _sString, Color _oColor )
		{
			TextBox_EncryptLog.SelectionColor = _oColor;
			TextBox_EncryptLog.AppendText( $"{_sString}\n" );
			TextBox_EncryptLog.SelectionStart = TextBox_EncryptLog.Text.Length;
			TextBox_EncryptLog.ScrollToCaret();
		}

		private void DeleteFileOrFolderInTreeView( ref TreeView _oTreeView, TreeNode _oSelectedNode )
		{
			bool bPacked = _oTreeView == TreeView_EncryptedFiles;

			if( _oSelectedNode.Nodes.Count > 0 )
				Directory.Delete( GetFileCompletePath( _oSelectedNode, bPacked ) );
			else
				File.Delete( GetFileCompletePath( _oSelectedNode, bPacked ) );

			if( bPacked )
			{
				_oSelectedNode.Checked = false;
				CheckDirectoryChildren( _oSelectedNode );
			}

			_oTreeView.BeginUpdate();
			_oTreeView.Nodes.Remove( _oSelectedNode );
			_oTreeView.EndUpdate();
		}

		private string GetFileCompletePath( TreeNode _oNode, bool _bPacked )
		{
			if( _bPacked )
				return m_oSelectedProject.m_sPackedDataPath + "\\" + _oNode.FullPath;

			return m_oSelectedProject.m_sDataPath + "\\" + _oNode.FullPath;
		}


		////////////////////////// FORM EVENTS //////////////////////////

		private void Button_ClearSelection_Click( object sender, EventArgs e )
		{
			if( m_oCheckedItems == null )
			{
				Label_CheckItems.Text = $"0 Selected";
				return;
			}

			m_bProcessingCheckboxes = true;
			foreach( TreeNode oNode in TreeView_RawFiles.Nodes )
			{
				oNode.Checked = false;

				if( oNode.Checked )
					m_oCheckedItems.Add( oNode );
				else /*if( m_oCheckedItems.Find( x => x == oNode ) != null )*/
					m_oCheckedItems.Remove( oNode );

				CheckDirectoryChildren( oNode );
			}
			m_bProcessingCheckboxes = false;

			Button_BuildSelected.Enabled = false;
			Button_ClearSelection.Enabled = false;

			Label_CheckItems.Text = $"{m_oCheckedItems.Count} Selected";
		}

		private void Button_BuildAll_Click( object sender, EventArgs e )
		{
			TextBox_EncryptLog.Clear();
			ProgressBar_Build.Value = 0;
			ProgressBar_Build.Maximum = m_oRawFiles.Count;
			Encrypter.EncryptFiles( m_oRawFiles, m_oSelectedProject.m_sDataPath, m_oSelectedProject.m_sPackedDataPath, CheckBox_ForceBuild.Checked, OnFileEncryptionEvent );
		}

		private void Button_BuildSelected_Click( object sender, EventArgs e )
		{
			TextBox_EncryptLog.Clear();
			ProgressBar_Build.Value = 0;
			ProgressBar_Build.Maximum = m_oCheckedItems.Count;
			Encrypter.EncryptFiles( GetCheckedFiles(), m_oSelectedProject.m_sDataPath, m_oSelectedProject.m_sPackedDataPath, CheckBox_ForceBuild.Checked, OnFileEncryptionEvent );
		}

		private void Button_RefreshRawFiles_Click( object sender, EventArgs e )
		{
			UpdateTreeView( ref TreeView_RawFiles, m_oSelectedProject.m_sDataPath );

			m_oRawFiles.Clear();
			GetAllFiles( ref m_oRawFiles, TreeView_RawFiles.Nodes );

			if( m_oCheckedItems != null )
				m_oCheckedItems.Clear();

			Label_CheckItems.Text = $"0 Selected";
		}

		private void TreeView_RawFiles_NodeMouseClick( object sender, TreeNodeMouseClickEventArgs e )
		{
			if( e.Button == MouseButtons.Right )
			{
				TreeView_RawFiles.SelectedNode = e.Node;
				TreeView_RawFiles.ContextMenuStrip = ContextMenu_RawFileNode;
			}
		}


		private void ContextMenuRaw_Delete_Click( object sender, EventArgs e )
		{
			DeleteFileOrFolderInTreeView( ref TreeView_RawFiles, TreeView_RawFiles.SelectedNode );
		}

		private void ContextMenuRaw_Encrypt_Click( object sender, EventArgs e )
		{
			List<String> oFile = new List<string>();
			oFile.Add( GetFileCompletePath( TreeView_RawFiles.SelectedNode, false ) );

			TextBox_EncryptLog.Clear();
			ProgressBar_Build.Value = 0;
			ProgressBar_Build.Maximum = 1;

			Encrypter.EncryptFiles( oFile, m_oSelectedProject.m_sDataPath, m_oSelectedProject.m_sPackedDataPath, CheckBox_ForceBuild.Checked, OnFileEncryptionEvent );
		}

		private void ContextMenuEncrypted_Delete_Click( object sender, EventArgs e )
		{
			DeleteFileOrFolderInTreeView( ref TreeView_EncryptedFiles, TreeView_EncryptedFiles.SelectedNode );
		}

		private void TreeView_EncryptedFiles_NodeMouseClick( object sender, TreeNodeMouseClickEventArgs e )
		{
			if( e.Button == MouseButtons.Right )
			{
				TreeView_EncryptedFiles.SelectedNode = e.Node;
				TreeView_EncryptedFiles.ContextMenuStrip = ContextMenu_EncryptedFileNode;
			}
		}

		private void ContextMenuEncrypted_Decrypt_Click( object sender, EventArgs e )
		{
			TextBox_EncryptLog.Clear();
			ProgressBar_Build.Value = 0;
			ProgressBar_Build.Maximum = 1;

			string sFile = GetFileCompletePath( TreeView_EncryptedFiles.SelectedNode, true );

			byte[] pFile = null;
			Encrypter.Decrypt_File( sFile, out pFile, Encrypter.IsTextFile( sFile ) );

			int iStartFileName = sFile.LastIndexOf( "\\" );

			if( iStartFileName >= 0 )
			{
				string sRawFileName = sFile.Substring( 0, iStartFileName + 1 ) + "Decrypted_" + sFile.Substring( iStartFileName + 1 );
				sFile = sRawFileName;
			}

			using( FileStream inFs = new FileStream( sFile, FileMode.Create ) )
			{
				inFs.Write( pFile, 0, pFile.Length );
				inFs.Close();
			}
		}
	}
}
