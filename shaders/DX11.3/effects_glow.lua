function normal( shader, t_base, t_second, t_detail )
  		shader:begin( "effects_glow_p", "effects_glow_p" )
		: blend		( true, blend.srcalpha, blend.one )
		: sorting	( 3, false )
		: zb            ( true, false )
		: fog		( false )
		shader:dx10texture	("s_base", t_base)
		shader:dx10texture	("s_position", "$user$position")

		shader:dx10sampler	("smp_base")
		shader:dx10sampler	("smp_nofilter")
end
