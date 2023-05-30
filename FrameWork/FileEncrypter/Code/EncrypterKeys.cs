using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileEncrypter.Code
{
    public static partial class Encrypter
    {
        static private Dictionary<string, string> m_oKeys = new Dictionary<string, string>()
        {
            { "The Shooting of Isaac: Rebirth", "<RSAKeyValue><Modulus>pSUEwMJVe2JnTBRm6vNJre4yvTaIXS/X+O3cwiNdVI3iOSpxsj3xNLawO5pov4QcSsL0gILUO6CQgv/do+BmHSp9/8ZE5JfnCOSf0zzogUBVqAD3nmbRkiKIeoBp1eUhFSNDjesg3eEkA3/4sFa1iHI278/NTUGoP6JvBowQTF0=</Modulus><Exponent>AQAB</Exponent><P>ymEGp/F0b3KzahVx8tkuHRm5Y6Nejc4xZ0nPq9P36jpYr38H1fVSM7ZTKi1M2yrBldLAmyD6Q2r/MNdPuWCFew==</P><Q>0OZ0mEAOuNgkOoXsO6zY4b6Ct6za1tA2roNyhz6rk1hAz7fF0AguXVJcHve/JX+lKTfc3TFPCr84h7ivc/4SBw==</Q><DP>B/E+3aBwPV93AxbMXgd/88xqpipnL5GEBtWyvL+Y3yxjBqApoo/6JeVl5iXY+lD3P/YBcRUePT8ZvMT2cAQFuQ==</DP><DQ>SSjsnIWSolnQFk7gtOWL70NxNDJtUd4JyIRwj7z7CddWm2iY9opm8nftm0rYvqE9jnnp9+1U6Uae3aloiQ7p8Q==</DQ><InverseQ>gMfx2YaDUXPIo6+hs43Aryv7oVgqn6+rEK+SdGZia2DGyjpqhGy7OHXeE7kTdVqZX27lc4Hz1rwYhQ1FI8I89w==</InverseQ><D>KQ5j+RwXLl6Peb2UvJKRXblbSekVy70KP2u7N1Sdblj7xeSEcaCup8FHXPoNUpeghQGk0ZI0+0H2y2qbCpPOLsnS7u16Qf4CYcgdk5Bo8SX47Wshy53ZZHnXza6EoBZYuOpW9TH7EcbJlI0w3qk5iT5HJY10Gs5dJdCBzw4+tc0=</D></RSAKeyValue>" },
        };

    }
}
