/* @M3EXPLORER_LICENSE_START@
 *
 * This file is part of the Multicube Explorer tool.
 *
 * Authors: Vittorio Zaccaria, Gianluca Palermo, Giovanni Mariani, Fabrizio Castro, Stefano Bolli (2009)
 * Copyright (c) 2008-2009, Politecnico di Milano, Universita' della Svizzera italiana
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * Neither the name of Politecnico di Milano nor Universita' della Svizzera Italiana 
 * nor the names of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This work was supported by the EC under grant MULTICUBE FP7-216693.
 * 
 * @M3EXPLORER_LICENSE_END@ */


/* @additional_authors @, Stefano Bolli (2009)@ */


#include <m3_doe.h>
#include <m3_parser.h>
#include <m3_shell_variables.h>


class m3_two_level_ff_extended_doe: public m3_doe
{
    public:
	vector<m3_point> generate_doe(m3_env *env);
	string get_information();
};

vector<m3_point> m3_two_level_ff_extended_doe::generate_doe(m3_env *env)
{

    int ds_size = env->current_design_space->ds_parameters.size();
    int max = (1 << ds_size) - 1; 
    int min = 0;
    int num_point;
    vector<m3_point> doe;

    if(!get_integer_from_variables(env,"num_generation_for_each_point",num_point)) 
    {
        num_point=1;
	prs_display_message("num_generation_for_each_point variable not found");	
    }

    for(int k=0; k<num_point; k++)
    {
        for(int j=min; j<=max; j++)
        {
            string res;
            m3_point actual_point;
            env->current_design_space->convert_two_level_factorial_representation_from_int(env, actual_point, j, res, TWO_LEVEL_FF_MODE_SCRAMBLED);
	    doe.push_back(actual_point);
        }

        (env->current_design_space->mask_opposite_level_index).erase((env->current_design_space->mask_opposite_level_index).begin(), 
	                                                                            (env->current_design_space->mask_opposite_level_index).end());
	(env->current_design_space->mask_opposite_level).erase((env->current_design_space->mask_opposite_level).begin(),
                                                                               (env->current_design_space->mask_opposite_level).end());
	(env->current_design_space->permutation_opposite_level_index).erase((env->current_design_space->permutation_opposite_level_index).begin(), 
	                                                                            (env->current_design_space->permutation_opposite_level_index).end());
	(env->current_design_space->permutation_opposite_level).erase((env->current_design_space->permutation_opposite_level).begin(),
	                                                                            (env->current_design_space->permutation_opposite_level).end());
    }
    return doe;
}

string m3_two_level_ff_extended_doe::get_information()
{
    return "Full factorial extended DoE";
}

extern "C" 
{
    m3_doe *doe_generate_doe()
    {
        return new m3_two_level_ff_extended_doe();
    }
}

