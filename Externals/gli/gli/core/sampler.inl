///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Image (gli.g-truc.net)
///
/// Copyright (c) 2008 - 2015 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file gli/core/sampler.inl
/// @date 2015-09-29 / 2015-10-03
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#include <glm/gtx/wrap.hpp>

namespace gli{
namespace detail
{
	template <typename T>
	inline T passThrought(T const & SampleCoord)
	{
		return SampleCoord;
	}
}//namespace detail

	inline sampler::sampler(wrap Wrap, filter Mip, filter Min)
		: Wrap(getFunc(Wrap))
		, Mip(Mip)
		, Min(Min)
	{}

	inline sampler::wrap_type sampler::getFunc(wrap WrapMode) const
	{
		static wrap_type Table[] =
		{
			glm::clamp,
			detail::passThrought,
			glm::repeat,
			glm::mirrorRepeat,
			glm::mirrorClamp,
			glm::mirrorClamp
		};
		static_assert(sizeof(Table) / sizeof(Table[0]) == WRAP_COUNT, "Table needs to be updated");

		return Table[WrapMode];
	}
}//namespace gli
