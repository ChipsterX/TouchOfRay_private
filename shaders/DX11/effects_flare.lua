function normal( shader, t_base, t_second, t_detail )

  		shader:begin( "ecb_fx_generic", "effects_flare_p" )
		: blend		( true, blend.srcalpha, blend.one )
		: sorting	( 3, false )
		: aref 		( true, 2 )
		: zb            ( false, false )
		: fog		( false )
		shader:dx10texture	("s_base", t_base)
		shader:dx10sampler	("smp_base")
end
