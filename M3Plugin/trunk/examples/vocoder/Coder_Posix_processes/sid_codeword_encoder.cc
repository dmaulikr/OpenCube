#ifndef _SID_CODEWORD_ENCODER_CC
#define _SID_CODEWORD_ENCODER_CC

extern "C" void *sid_encoding_fun(void *args);

void *sid_encoding_fun(void *args){

sid_fifos();
	
	bool aux_bit;
	unsigned int serial_bit;
	Word16	txdtx_ctrl_var;
#ifdef HW_ACCESS
	int* prm_out = (int*)0xBF000000;
#endif	
	BEGIN_PROF();
	
	while(true) {
		CH_MONITOR(txdtx_ctrl_var = txdtx_ctrl_vad_5->read();)
		if( (txdtx_ctrl_var& TX_SP_FLAG)==0) {
			for(serial_bit=0;serial_bit<244;serial_bit++) {
				CH_MONITOR(aux_bit = bit_stream_->read();)
				// encoding sid codeword
				if((serial_bit>44)&&(serial_bit<47)) aux_bit=true;
				if((serial_bit>47)&&(serial_bit<69)) aux_bit=true;
				if((serial_bit>93)&&(serial_bit<97)) aux_bit=true;
				if((serial_bit>97)&&(serial_bit<119)) aux_bit=true;
				if((serial_bit>147)&&(serial_bit<172)) aux_bit=true;
				if((serial_bit>195)&&(serial_bit<210)) aux_bit=true;
				if((serial_bit>211)&&(serial_bit<222)) aux_bit=true;
				// just bypass the bit
				HW_ACCESS_WRITE(prm_out,aux_bit);
			}
		} 
		else {
		// bypass the whole frame
			for(serial_bit=0;serial_bit<244;serial_bit++) {

				CH_MONITOR(aux_bit = bit_stream_->read();)

				HW_ACCESS_WRITE(prm_out,aux_bit);

			}
		}
		
		// sending VAD bit
	        if ((txdtx_ctrl_var & TX_VAD_FLAG) != 0){
			#ifdef DBG_MSG			
			printf("VAD=1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			#endif			
			HW_ACCESS_WRITE(prm_out,true);
		}
		else {
			#ifdef DBG_MSG			
			printf("VAD=0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			#endif			
			HW_ACCESS_WRITE(prm_out,false);
		}
		
		// sending SP bit
	        if ((txdtx_ctrl_var & TX_SP_FLAG) != 0) {
			#ifdef DBG_MSG			
			printf("SP=1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			#endif			
			HW_ACCESS_WRITE(prm_out,true);
		}
		else {
			#ifdef DBG_MSG
			printf("SP=0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			#endif
			HW_ACCESS_WRITE(prm_out,false);
		}		
	}

}//sid_encoding_fun

#endif 
