using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.IO;
using System.Diagnostics;


namespace FileEncrypter.Code
{
	using FileEncryptionCallback = System.Action<FileEncryptionEvent, string>;

	public enum FileEncryptionEvent
	{
		Encrypting,
		Skipped,
		FileDone,
		EncryptionDone,
	};

	public static partial class Encrypter
	{
		public enum CryptionTaskResult
		{
			eSuccess,
			eFileNotFound,
			eRSAKeyMismatch,
			eCount,
		};

		static CspParameters m_oCSPParams = new CspParameters();
		static RSACryptoServiceProvider m_oRSA = null;

		static string m_sRawFilesRootDirectory = "";
		static string m_sPackedFilesRootDirectory = "";
		static bool m_bForceBuild = false;

		static string m_sKeyName = "Key01";

		static private FileEncryptionCallback m_pFileEncryptedCallback = null;

		static public void CreateKeys( string _sKeyName = "" )
		{
			m_oCSPParams.KeyContainerName = _sKeyName.Length > 0 ? _sKeyName : m_sKeyName;
			m_oRSA = new RSACryptoServiceProvider( m_oCSPParams );
			m_oRSA.PersistKeyInCsp = true;
			//Debug.WriteLine(m_oRSA.ToXmlString(true));
		}

		static public void InitKeys( string _sKeyName )
		{
			if (m_oKeys[_sKeyName] == "")
				CreateKeys(_sKeyName);
			else
			{
				m_oCSPParams.KeyContainerName = _sKeyName;
				m_oRSA = new RSACryptoServiceProvider(m_oCSPParams);
				m_oRSA.FromXmlString(m_oKeys[_sKeyName]);
				m_oRSA.PersistKeyInCsp = true;
			}
		}

		static public async void EncryptAssets( string _sSrcRootDirectory, string _sDstRootDirectory, bool _bForceBuild, FileEncryptionCallback _pFileEncryptedCallback )
		{
			if( Directory.Exists( _sSrcRootDirectory ) == false )
				return;

			if( Directory.Exists( _sDstRootDirectory ) == false )
				Directory.CreateDirectory( _sDstRootDirectory );

			if( m_oRSA == null )
				CreateKeys();

			m_sRawFilesRootDirectory = _sSrcRootDirectory;
			m_sPackedFilesRootDirectory = _sDstRootDirectory;
			m_bForceBuild = _bForceBuild;
			m_pFileEncryptedCallback = _pFileEncryptedCallback;

			await Task.Run( () => EncryptFolder( _sSrcRootDirectory ) );
		}

		static public async void EncryptFiles( List<string> _oFiles, string _sSrcRootDirectory, string _sDstRootDirectory, bool _bForceBuild, FileEncryptionCallback _pFileEncryptedCallback )
		{
			if( _oFiles == null )
				return;

			if( Directory.Exists( _sDstRootDirectory ) == false )
				Directory.CreateDirectory( _sDstRootDirectory );

			if( m_oRSA == null )
				CreateKeys();

			m_sRawFilesRootDirectory = _sSrcRootDirectory;
			m_sPackedFilesRootDirectory = _sDstRootDirectory;
			m_bForceBuild = _bForceBuild;
			m_pFileEncryptedCallback = _pFileEncryptedCallback;

			foreach( string sFile in _oFiles )
			{
				string sDstFolder = _sDstRootDirectory + sFile.Substring( _sSrcRootDirectory.Length );
				sDstFolder = sDstFolder.Substring( 0, sDstFolder.LastIndexOf( "\\" ) );
				await Task.Run( () => Encrypt_File( sFile, sDstFolder ) );
			}

			if( m_pFileEncryptedCallback != null )
				m_pFileEncryptedCallback( FileEncryptionEvent.EncryptionDone, "" );
		}

		static private void EncryptFolder( string _sSrcFolder )
		{
			if( Directory.Exists( _sSrcFolder ) == false )
				return;

			string[] pDirectories = Directory.GetDirectories( _sSrcFolder );

			foreach( string sDirectory in pDirectories )
			{
				if( sDirectory.IndexOf( "\\Packed" ) >= 0 )
					continue;

				EncryptFolder( sDirectory );
			}

			string sDstFolder = m_sPackedFilesRootDirectory + _sSrcFolder.Substring( m_sRawFilesRootDirectory.Length );

			string[] pFiles = Directory.GetFiles( _sSrcFolder );

			foreach( string sFile in pFiles )
			{
				Encrypt_File( sFile, sDstFolder );
			}

			if( m_pFileEncryptedCallback != null )
				m_pFileEncryptedCallback( FileEncryptionEvent.EncryptionDone, "" );
		}

		static private void Encrypt_File( string _sSrcFile, string _sDstFolder )
		{
			// Create instance of Aes for symmetric encryption of the data.
			Aes oAes = Aes.Create();
			ICryptoTransform transform = oAes.CreateEncryptor();

			// Use RSACryptoServiceProvider to encrypt the AES key.
			// rsa is previously instantiated:
			//    rsa = new RSACryptoServiceProvider(cspp);
			byte[] pKeyEncrypted = m_oRSA.Encrypt( oAes.Key, false );

			// Create byte arrays to contain the length values of the key and IV.
			byte[] pLenK = new byte[4];
			byte[] pLenIV = new byte[4];

			pLenK = BitConverter.GetBytes( pKeyEncrypted.Length );
			pLenIV = BitConverter.GetBytes( oAes.IV.Length );

			// Write the following to the FileStream for the encrypted file (outFs):
			// - length of the key
			// - length of the IV
			// - ecrypted key
			// - the IV
			// - the encrypted cipher content

			string sNewFileName = _sDstFolder + "\\" + GetEncryptedFileName( _sSrcFile );

			Directory.CreateDirectory( _sDstFolder );

			if( File.Exists( sNewFileName ) && m_bForceBuild == false )
			{
				if( m_pFileEncryptedCallback != null )
					m_pFileEncryptedCallback( FileEncryptionEvent.Skipped, _sSrcFile );
				return;
			}

			using( FileStream outFs = new FileStream( sNewFileName, FileMode.Create ) )
			{
				outFs.Write( pLenK, 0, 4 );
				outFs.Write( pLenIV, 0, 4 );
				outFs.Write( pKeyEncrypted, 0, pKeyEncrypted.Length );
				outFs.Write( oAes.IV, 0, oAes.IV.Length );

				// Now write the cipher text using a CryptoStream for encrypting.
				using( CryptoStream outStreamEncrypted = new CryptoStream( outFs, transform, CryptoStreamMode.Write ) )
				{
					// By encrypting a chunk at a time, you can save memory and accommodate large files.
					int count = 0;

					// blockSizeBytes can be any arbitrary size.
					int blockSizeBytes = oAes.BlockSize / 8;
					byte[] data = new byte[blockSizeBytes];
					int bytesRead = 0;

					try
					{
						if( IsTextFile( _sSrcFile ) )
						{
							using( StreamReader oReader = new StreamReader( _sSrcFile ) )
							{
								string sRead = oReader.ReadLine();

								while( sRead != null )
								{
									sRead += "\r\n";

									sRead = ConvertReferencedDataPaths( sRead );

									byte[] pRead = Encoding.ASCII.GetBytes( sRead );
									outStreamEncrypted.Write( pRead, 0, pRead.Length );
									bytesRead += count;

									sRead = oReader.ReadLine();
								}

								oReader.Close();
							}
						}
						else
						{
							using( FileStream inFs = new FileStream( _sSrcFile, FileMode.Open ) )
							{
								if( m_pFileEncryptedCallback != null )
									m_pFileEncryptedCallback( FileEncryptionEvent.Encrypting, _sSrcFile );
								Debug.WriteLine( $"Encrypting file: {_sSrcFile}." );

								do
								{
									count = inFs.Read( data, 0, blockSizeBytes );
									outStreamEncrypted.Write( data, 0, count );
									bytesRead += count;

								}
								while( count > 0 );

								inFs.Close();
							}
						}
					}
					catch( Exception ex )
					{
						Debug.WriteLine( $"Couldn't open raw file: {ex}." );
					}

					outStreamEncrypted.FlushFinalBlock();
					outStreamEncrypted.Close();

					if( m_pFileEncryptedCallback != null )
						m_pFileEncryptedCallback( FileEncryptionEvent.FileDone, _sSrcFile );
				}
				outFs.Close();
			}
		}

		static public CryptionTaskResult Decrypt_File( string _sSrcFile, out byte[] _pResult, bool _bTextFile = false )
		{
			Debug.WriteLine( $"Decrypting file: {_sSrcFile}" );

			if( m_oRSA == null )
				CreateKeys();

			_pResult = new byte[1];
			// Create instance of Aes for symetric decryption of the data.
			Aes aes = Aes.Create();

			// Create byte arrays to get the length of the encrypted key and IV.
			// These values were stored as 4 bytes each at the beginning of the encrypted package.
			byte[] LenK = new byte[4];
			byte[] LenIV = new byte[4];

			// Use FileStream objects to read the encrypted file (inFs) and save the decrypted file (outFs).
			FileStream inFs = null;

			try
			{
				inFs = new FileStream( _sSrcFile, FileMode.Open );
			}
			catch( Exception ex )
			{
				Debug.WriteLine( ex );
				return CryptionTaskResult.eFileNotFound;
			}

			inFs.Seek( 0, SeekOrigin.Begin );
			inFs.Seek( 0, SeekOrigin.Begin );
			inFs.Read( LenK, 0, 3 );
			inFs.Seek( 4, SeekOrigin.Begin );
			inFs.Read( LenIV, 0, 3 );

			// Convert the lengths to integer values.
			int lenK = BitConverter.ToInt32( LenK, 0 );
			int lenIV = BitConverter.ToInt32( LenIV, 0 );

			// Determine the start postition of the ciphter text (startC) and its length(lenC).
			int startC = lenK + lenIV + 8;
			int lenC = (int)inFs.Length - startC;

			// Create the byte arrays for the encrypted Aes key, the IV, and the cipher text.
			byte[] KeyEncrypted = new byte[lenK];
			byte[] IV = new byte[lenIV];

			// Extract the key and IV starting from index 8 after the length values.
			inFs.Seek( 8, SeekOrigin.Begin );
			inFs.Read( KeyEncrypted, 0, lenK );
			inFs.Seek( 8 + lenK, SeekOrigin.Begin );
			inFs.Read( IV, 0, lenIV );

			byte[] KeyDecrypted = null;
			ICryptoTransform transform = null;

			try
			{
				// Use RSACryptoServiceProvider to decrypt the AES key.
				KeyDecrypted = m_oRSA.Decrypt( KeyEncrypted, false );
				// Decrypt the key.
				transform = aes.CreateDecryptor( KeyDecrypted, IV );
			}
			catch( Exception ex )
			{
				Debug.WriteLine( ex );
				return CryptionTaskResult.eRSAKeyMismatch;
			}

			int count = 0;
			int offset = 0;

			// blockSizeBytes can be any arbitrary size.
			int blockSizeBytes = aes.BlockSize / 8;
			byte[] data = new byte[blockSizeBytes];
			MemoryStream memStream = new MemoryStream();
			// By decrypting a chunk a time, you can save memory and accommodate large files.

			// Start at the beginning of the cipher text.
			inFs.Seek( startC, SeekOrigin.Begin );

			using( CryptoStream outStreamDecrypted = new CryptoStream( memStream, transform, CryptoStreamMode.Write ) )
			{
				do
				{
					count = inFs.Read( data, 0, blockSizeBytes );
					offset += count;
					outStreamDecrypted.Write( data, 0, count );
				}
				while( count > 0 );

				outStreamDecrypted.FlushFinalBlock();

				if( _bTextFile )
				{
					StreamReader oReader = new StreamReader( memStream );
					memStream.Seek( 0, SeekOrigin.Begin );
					string struc = oReader.ReadToEnd();
					struc = struc.Replace( '\r'.ToString(), string.Empty );
					struc += '\0';
					_pResult = Encoding.ASCII.GetBytes( struc );
				}
				else
					_pResult = memStream.ToArray();

				try
				{
					outStreamDecrypted.Close();
				}
				catch( Exception ex )
				{
					Debug.WriteLine( $"close: {ex}." );
				}

			}

			inFs.Close();


			return CryptionTaskResult.eSuccess;
		}

		static private string GetEncryptedFileName( string _sSrcFile )
		{
			int iStartFileName = _sSrcFile.LastIndexOf( "\\" );

			if( iStartFileName < 0 )
				return "";

			string sRawFileName = _sSrcFile.Substring( iStartFileName + 1 );

			int iExtensionIndex = sRawFileName.LastIndexOf( "." );

			if( iExtensionIndex < 0 )
				return sRawFileName;

			return sRawFileName.Substring( 0, iExtensionIndex ) + GetEncryptedFileExtension( sRawFileName.Substring( iExtensionIndex ) );
		}

		static public bool IsTextFile( string _sFile )
		{
			string sExtension = GetFileExtension( _sFile );

			if( sExtension == ".txt" || sExtension == ".cfg" || sExtension == ".anm" || sExtension == ".anm2" || sExtension == ".xml" )
				return true;

			if( sExtension == ".ini" || sExtension == ".frag" || sExtension == ".vert" || sExtension == ".sdrf" || sExtension == ".sdrv" )
				return true;

			if( sExtension == ".fnt" )
				return true;

			return false;
		}

		static private string GetFileExtension( string _sFile )
		{
			int iExtensionIndex = _sFile.LastIndexOf( "." );

			if( iExtensionIndex < 0 )
				return _sFile;

			return _sFile.Substring( iExtensionIndex );
		}

		static private string GetEncryptedFileExtension( string _sRawExtension )
		{
			if( _sRawExtension == ".ogg" || _sRawExtension == ".wav" )
				return ".snd";

			if( _sRawExtension == ".anm" || _sRawExtension == ".anm2" )
				return ".anm";

			if( _sRawExtension == ".png" )
				return ".img";

			if( _sRawExtension == ".fnt" || _sRawExtension == ".ttf" )
				return ".fnt";

			if( _sRawExtension == ".ico" )
				return ".icn";

			if( _sRawExtension == ".frag" )
				return ".sdrf";

			if( _sRawExtension == ".vert" )
				return ".sdrv";

			if( _sRawExtension == ".xml" )
				return ".cfg";

			return _sRawExtension;
		}

		static private string ConvertReferencedDataPaths( string _sLine )
		{
			if( _sLine.IndexOf( "<" ) < 0 || _sLine.LastIndexOf( ">" ) < 0 )
				return _sLine;

			int iQuoteIndex = 0;
			string sText = null;
			string sNewLine = _sLine;

			do
			{
				sText = GetTextBetweenQuotes( sNewLine, ref iQuoteIndex );

				if( sText != null )
				{
					AdaptRootDataPath( ref sNewLine, ref sText, iQuoteIndex );
					ChangeFileExtention( ref sNewLine, ref sText, iQuoteIndex );

					iQuoteIndex += sText.Length + 1;
				}
			}
			while( sText != null );

			return sNewLine;
		}

		static private string GetTextBetweenQuotes( string _sLine, ref int _iCurrentQuoteIndex )
		{
			int iFirstQuoteIndex = _sLine.IndexOf( "\"", _iCurrentQuoteIndex );

			if( iFirstQuoteIndex < 0 )
				return null;

			_iCurrentQuoteIndex = iFirstQuoteIndex + 1;
			
			int iSecondQuoteIndex = _sLine.IndexOf( "\"", _iCurrentQuoteIndex );

			if( iSecondQuoteIndex < 0 )
				return null;

			return _sLine.Substring( _iCurrentQuoteIndex, iSecondQuoteIndex - _iCurrentQuoteIndex );
		}

		static private void AdaptRootDataPath( ref string _sLine, ref string _sText, int iQuoteIndex )
		{
			int iStringIndex = _sText.IndexOf( "/Data/" );

			if( iStringIndex < 0 )
				iStringIndex = _sText.IndexOf( "\\Data\\" );

			if( iStringIndex < 0 )
				return;

			char cSlash = _sText[iStringIndex];
			string sNewText = _sText.Insert( iStringIndex + 6, "Packed" + cSlash );

			_sLine = _sLine.Substring( 0, iQuoteIndex ) + sNewText + _sLine.Substring( iQuoteIndex + _sText.Length );
			_sText = sNewText;
		}

		static private void ChangeFileExtention( ref string _sLine, ref string _sText, int iQuoteIndex )
		{
			int iDotIndex = _sText.LastIndexOf( "." );

			if( iDotIndex < 0 )
				return;

			string sNewText = _sText.Substring( 0, iDotIndex ) + GetEncryptedFileExtension( _sText.Substring( iDotIndex ) );

			_sLine = _sLine.Substring( 0, iQuoteIndex ) + sNewText + _sLine.Substring( iQuoteIndex + _sText.Length );
		}
	}
}
