#pragma once

#include <string>

#include "FZN/Defines.h"


namespace fzn
{
	namespace Shaders
	{
		const std::string ColorOverlay_Frag =
			ToString
			(
				uniform sampler2D texture;
				uniform vec4 tintColor;

				void main()
				{
					vec4 pixel = texture2D( texture, gl_TexCoord[ 0 ].xy );

					pixel = gl_Color * pixel;

					vec4 finalTintColor = tintColor;
					finalTintColor.a = pixel.a;

					float blendAlpha = tintColor.a;

					gl_FragColor = pixel * ( 1.0 - blendAlpha ) + finalTintColor * blendAlpha;
				}
			);

		const std::string ColorOverlay_Vert =
			ToString
			(
				void main()
				{
					gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
					gl_TexCoord[ 0 ] = gl_TextureMatrix[ 0 ] * gl_MultiTexCoord0;
					gl_FrontColor = gl_Color;
				}
			);
	}
}
