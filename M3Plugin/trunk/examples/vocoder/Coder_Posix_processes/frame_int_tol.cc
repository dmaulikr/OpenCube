#ifndef _FRAME_INT_TOL_CC
#define _FRAME_INT_TOL_CC

extern "C" void *frame_int_tol(void *args);

void *frame_int_tol(void *args){

frame_int_fifos();

	bool dtx_mode_var; 
	Word16 txdtx_ctrl_var; 
 	bool reset; 
	Word16 i; 
	// signal buffer for preprocessed (prefiltered signal) 
	// It is stored the length corresponding to the FRAME 
	// plus las previous M samples (needed for residual cal 
	// culation (correlation 
	// done (previous to Auto correlation. 
	// After each frame processing the last L_WINDOW-L_FRAME 
	// samples are shifted to the begining of the buffer to 
	// give pass to the next L_FRAME samples of next frame 
	Word16 x[L_FRAME+M]; 
	Word16 lsp_old[M];		// LSPs at previous 4th subframe 
	Word16 lsp_mid[M];		// LSPs at 2nd subframe 
	Word16 lsp_new[M];		// LSPs at 4th subframe 
	Word16 A_t[(MP1) * 4];  // A(z) unquantized for the 4 subframes  
	Word16 *A;					// Pointer on A_t 
	Word16 Ap1[MP1];		// Ap(z) with spectral expansion          
	Word16 Ap2[MP1];		// Ap(z) with spectral expansion          
	Word16 old_wsp[L_FRAME + PIT_MAX]; 
	Word16 *wsp; 
	Word16 mem_w[M]; 
 	Word16 T_ol; 
 
	BEGIN_PROF(); 
 #ifndef _CODER_2PORTS
	#ifdef DBG_MSG						 
		printf("FRAME_INT_LSP: Before reading dtx_mode\n"); 
	#endif 
	CH_MONITOR(dtx_mode_var = dtx_mode_1->read();) 
	#ifdef DBG_MSG						
		printf("FRAME_INT_LSP: After reading dtx_mode\n"); 
	#endif 
 
	#else 
 
		#ifdef DTX_MODE	 
			dtx_mode_var = DTX_MODE_ENABLED; 
		#else 
			dtx_mode_var = DTX_MODE_DISABLED; 
		#endif 
 
	#endif 
 
	while(true) { 
		#ifdef DBG_MSG		 
			printf("FRAME_INT_TOL:------------------HOMING RESET---------------------- \n"); 
		#endif		 
		// INBAND-RESET LOOP 
		// speech window buffer (only necessary to iniialize first M samples) 
		Set_zero (x,M); 
		// Homing reset Initialization & Init_Coder_12k 
		wsp = old_wsp + PIT_MAX;	 
		Set_zero (old_wsp, PIT_MAX);	 
		Set_zero (mem_w, M); 
 		while(true) { 
			 
			// MAIN LOOP 
			#ifdef DBG_MSG 
				printf("FRAME_INT_TOL: Reading non quantizied lsps\n"); 
			#endif	 
			for(i=0;i<M;i++) { 
				CH_MONITOR(lsp_old[i] = lsp->read();) 
			} 
	 
			// Receive an place non-quantizied LPC coefficients of 2nd andt third subframe 
			for(i=0;i<MP1;i++) { 
				CH_MONITOR(A_t[MP1+i] = A_24->read();) 
			} 
	 
			for(i=0;i<MP1;i++) { 
				CH_MONITOR(A_t[3*MP1+i] = A_24->read();) 
			} 
			 
			for(i=0;i<M;i++) { 
				CH_MONITOR(lsp_mid[i] = lsp->read();) 
			} 
	 
			for(i=0;i<M;i++) { 
				CH_MONITOR(lsp_new[i] = lsp->read();) 
			} 
			 
			#ifdef DBG_MSG 
				printf("FRAME_INT_TOL: lsps readed. Interpolation of 1st and 3th LPC coefficients.\n"); 
			#endif	 
			// Interpolate non quantizied lsp 
			Int_lpc2 (lsp_old, lsp_mid, lsp_new, A_t); 
			for(i=0;i<(4*MP1);i++) { 
				CH_MONITOR(A_->write(A_t[i]);) 
			} 
			 
			// txdtx_ctrl_var its is not readed till is strictly necessary, just here 
			CH_MONITOR(txdtx_ctrl_var = txdtx_ctrl_vad_2->read();) 
			 
			if ((txdtx_ctrl_var & TX_SP_FLAG) == 0) { 
				// Necessary in the frame_lsp module for assigning to the Aq 
				for (i = 0; i < MP1; i++) { 
					CH_MONITOR(A_13->write(A_t[i]);) 
				} 
				for (i = 0; i < MP1; i++) { 
					CH_MONITOR(A_13->write(A_t[i+2*MP1]);) 
				} 
			} 
			 
			//---------------------------------------------------------------------- 
			//  - Find the weighted input speech wsp[] for the whole speech frame     
			//  - Find the open-loop pitch delay for first 2 subframes                  
			//  - Find the open-loop pitch delay for last 2 subframes                 
			//---------------------------------------------------------------------- 
	 
			for(i=M; i<(M+L_FRAME); i++) { 
				CH_MONITOR(x[i] = preproc_sample_2->read();) 
			} 
			 
			A = A_t;                                                      
			for (i = 0; i < L_FRAME; i += L_SUBFR) 
			{ 
				Weight_Ai (A, F_gamma1, Ap1); 
				Weight_Ai (A, F_gamma2, Ap2); 
	 
				Residu (Ap1, &x[i+M], &wsp[i], L_SUBFR); 
		 
				Syn_filt (Ap2, &wsp[i], &wsp[i], L_SUBFR, mem_w, 1); 
		 
				A += MP1;                                                 
			} 
			// Here data can already been shifted (previniendo una nueva partici�n que permitiera 
			// de nuevo el volcado de datos) 
			Copy (&x[L_FRAME], x, M); 
	 
			// Find open loop pitch lag for first two subframes 
		 
			T_ol = Pitch_ol (wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2); 
			 
			// transfer open loop pitch parameter for the two first subframes 
			if (dtx_mode_var) { 
				#ifdef DBG_MSG								 
					printf("FRAME_INT_TOL: Sending 1st lag.\n"); 
				#endif 
				CH_MONITOR(lags_->write(T_ol);) 
			} 
			CH_MONITOR(tol->write(T_ol);) 
			 
			// Find open loop pitch lag for last two subframes 
			T_ol = Pitch_ol (&wsp[L_FRAME_BY2], PIT_MIN, PIT_MAX, L_FRAME_BY2); 
			 
			// transfer open loop pitch parameter for the two first subframes 
			if(dtx_mode_var) { 
				#ifdef DBG_MSG				 
					printf("FRAME_INT_TOL: Sending 2nd lag.\n"); 
				#endif 
				CH_MONITOR(lags_->write(T_ol);) 
			} 
			CH_MONITOR(tol->write(T_ol);) 
			 
			// After each frame processing 
			Copy (&old_wsp[L_FRAME], &old_wsp[0], PIT_MAX); 
			 
			// CHECK OF IN-BAND RESET (lectura del canal siempre_hecha) 
			// the read is separated in order to avoid the break affects 
			// counters update when CH_MONITOR macro is used at SW_GENERATION 
			// level for profiling!! 
			CH_MONITOR(reset = inband_reset_2->read();) 
			if(reset) break; 
								 
			// En los SC_THREAD la estructura break permite salir del bucle 
			// principar y saltar al bucle de reset. Este contiene las sentencias 
			// de inicializaci�n (en nuestro caso las de reset en banda) Si hay alguna 
			// sentencia incluida en el reset inicial, pero no en el reset en banda, 
			// se antepone al inicio del reset en banda. 
			// La estructura mediante bucles de reset y main y break tiene una 
			// coincidencia grande con respecto al estilo de comportamiento en VHDL y 
			// una traslaci�n pr�cticamente directa al SystemC de comportamiento 
			// (eliminado el lazo de reset, usando el watching en los SC_CTHREAD...) 
			 
			//		La estructura if(in_band_reset->read()) goto inband_reset_label; no se usar� 
			//	para evitar cualquier referencia a lenguage no estructurado.		 
 
		}	// END MAIN LOOP 
	}	// END INBAND-RESET LOOP 


}//frame_int_tol
#endif 
