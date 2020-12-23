// 
// Politecnico di Milano
// Code created using PandA - Version: PandA 0.9.6 - Revision 5e5e306b86383a7d85274d64977a3d71fdcff4fe-HEAD - Date 2020-12-16T13:23:18
// bambu executed with: bambu --memory-allocation-policy=NO_BRAM --channels-type=MEM_ACC_11 --memory-ctrl-type=D21 -DLOCOD_FPGA --top-fname=addition ../main.c 
// 
// Send any bug to: panda-info@polimi.it
// ************************************************************************
// The following text holds for all the components tagged with PANDA_LGPLv3.
// They are all part of the BAMBU/PANDA IP LIBRARY.
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with the PandA framework; see the files COPYING.LIB
// If not, see <http://www.gnu.org/licenses/>.
// ************************************************************************

`ifdef __ICARUS__
  `define _SIM_HAVE_CLOG2
`endif
`ifdef VERILATOR
  `define _SIM_HAVE_CLOG2
`endif
`ifdef MODEL_TECH
  `define _SIM_HAVE_CLOG2
`endif
`ifdef VCS
  `define _SIM_HAVE_CLOG2
`endif
`ifdef NCVERILOG
  `define _SIM_HAVE_CLOG2
`endif
`ifdef XILINX_SIMULATOR
  `define _SIM_HAVE_CLOG2
`endif
`ifdef XILINX_ISIM
  `define _SIM_HAVE_CLOG2
`endif

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>, Christian Pilato <christian.pilato@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module constant_value(out1);
  parameter BITSIZE_out1=1, value=1'b0;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  assign out1 = value;
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module register_SE(clock, reset, in1, wenable, out1);
  parameter BITSIZE_in1=1, BITSIZE_out1=1;
  // IN
  input clock;
  input reset;
  input [BITSIZE_in1-1:0] in1;
  input wenable;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  
  reg [BITSIZE_out1-1:0] reg_out1 =0;
  assign out1 = reg_out1;
  always @(posedge clock)
    if (wenable)
      reg_out1 <= in1;
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module MEMORY_CTRL_D21(clock, start_port, in1, in2, in3, in4, sel_LOAD, sel_STORE, done_port, out1, Min_oe_ram, Mout_oe_ram, Min_we_ram, Mout_we_ram, Min_addr_ram, Mout_addr_ram, M_Rdata_ram, Min_Wdata_ram, Mout_Wdata_ram, Min_data_ram_size, Mout_data_ram_size, M_DataRdy);
  parameter BITSIZE_in1=1, BITSIZE_in2=1, BITSIZE_in3=1, BITSIZE_out1=1, BITSIZE_Min_addr_ram=1, BITSIZE_Mout_addr_ram=1, BITSIZE_M_Rdata_ram=8, BITSIZE_Min_Wdata_ram=8, BITSIZE_Mout_Wdata_ram=8, BITSIZE_Min_data_ram_size=1, BITSIZE_Mout_data_ram_size=1;
  // IN
  input clock;
  input start_port;
  input [BITSIZE_in1-1:0] in1;
  input [BITSIZE_in2-1:0] in2;
  input [BITSIZE_in3-1:0] in3;
  input in4;
  input sel_LOAD;
  input sel_STORE;
  input Min_oe_ram;
  input Min_we_ram;
  input [BITSIZE_Min_addr_ram-1:0] Min_addr_ram;
  input [BITSIZE_M_Rdata_ram-1:0] M_Rdata_ram;
  input [BITSIZE_Min_Wdata_ram-1:0] Min_Wdata_ram;
  input [BITSIZE_Min_data_ram_size-1:0] Min_data_ram_size;
  input M_DataRdy;
  // OUT
  output done_port;
  output [BITSIZE_out1-1:0] out1;
  output Mout_oe_ram;
  output Mout_we_ram;
  output [BITSIZE_Mout_addr_ram-1:0] Mout_addr_ram;
  output [BITSIZE_Mout_Wdata_ram-1:0] Mout_Wdata_ram;
  output [BITSIZE_Mout_data_ram_size-1:0] Mout_data_ram_size;
  wire  [BITSIZE_in2-1:0] tmp_addr;
  reg [BITSIZE_out1-1:0] delayed_out1 =0;
  reg delayed_M_DataRdy =0;
  reg delayed_sel_LOAD =0;
  reg delayed_sel_STORE =0;
  reg [BITSIZE_in1-1:0] delayed_in1 =0;
  reg [BITSIZE_in2-1:0] delayed_in2 =0;
  reg [BITSIZE_in3-1:0] delayed_in3 =0;
  
  always @(posedge clock)
    delayed_sel_LOAD <= sel_LOAD&!M_DataRdy&!delayed_M_DataRdy;
  always @(posedge clock)
    delayed_sel_STORE <= sel_STORE&!M_DataRdy&!delayed_M_DataRdy;
  always @(posedge clock)
    delayed_in1 <= in1;
  always @(posedge clock)
    delayed_in2 <= in2;
  always @(posedge clock)
    delayed_in3 <= in3;
  
  assign tmp_addr = delayed_in2;
  assign Mout_addr_ram = (delayed_sel_LOAD || delayed_sel_STORE) ? tmp_addr : Min_addr_ram;
  assign Mout_oe_ram = delayed_sel_LOAD ? 1'b1 : Min_oe_ram;
  assign Mout_we_ram = delayed_sel_STORE ? 1'b1 : Min_we_ram;
  always @(posedge clock)
    delayed_out1 <= M_Rdata_ram;
  assign out1 = delayed_out1;
  assign Mout_Wdata_ram = delayed_sel_STORE ? delayed_in1 : Min_Wdata_ram;
  assign Mout_data_ram_size = delayed_sel_STORE || delayed_sel_LOAD ? delayed_in3[BITSIZE_in3-1:0] : Min_data_ram_size;
  always @(posedge clock)
    delayed_M_DataRdy <= M_DataRdy & (delayed_sel_STORE || delayed_sel_LOAD);
  assign done_port = delayed_M_DataRdy;
  // Add assertion here
  // psl default clock = (posedge clock);
  // psl ERROR_Min_we_ram_Min_oe_ram: assert never {Min_we_ram && Min_oe_ram};
  // psl ERROR_LOAD_Min_oe_ram: assert never {delayed_sel_LOAD && Min_oe_ram};
  // psl ERROR_STORE_Min_we_ram: assert never {delayed_sel_STORE && Min_we_ram};
  // psl ERROR_STORE_LOAD: assert never {sel_STORE && sel_LOAD};
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module plus_expr_FU(in1, in2, out1);
  parameter BITSIZE_in1=1, BITSIZE_in2=1, BITSIZE_out1=1;
  // IN
  input signed [BITSIZE_in1-1:0] in1;
  input signed [BITSIZE_in2-1:0] in2;
  // OUT
  output signed [BITSIZE_out1-1:0] out1;
  assign out1 = in1 + in2;
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2013-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module bus_merger(in1, out1);
  parameter BITSIZE_in1=1, PORTSIZE_in1=2, BITSIZE_out1=1;
  // IN
  input [(PORTSIZE_in1*BITSIZE_in1)+(-1):0] in1;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  
  function [BITSIZE_out1-1:0] merge;
    input [BITSIZE_in1*PORTSIZE_in1-1:0] m;
    reg [BITSIZE_out1-1:0] res;
    integer i1;
  begin
    res={BITSIZE_in1{1'b0}};
    for(i1 = 0; i1 < PORTSIZE_in1; i1 = i1 + 1)
    begin
      res = res | m[i1*BITSIZE_in1 +:BITSIZE_in1];
    end
    merge = res;
  end
  endfunction
  
  assign out1 = merge(in1);
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module IUdata_converter_FU(in1, out1);
  parameter BITSIZE_in1=1, BITSIZE_out1=1;
  // IN
  input signed [BITSIZE_in1-1:0] in1;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  generate
  if (BITSIZE_out1 <= BITSIZE_in1)
  begin
    assign out1 = in1[BITSIZE_out1-1:0];
  end
  else
  begin
    assign out1 = {{(BITSIZE_out1-BITSIZE_in1){in1[BITSIZE_in1-1]}},in1};
  end
  endgenerate
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module UUdata_converter_FU(in1, out1);
  parameter BITSIZE_in1=1, BITSIZE_out1=1;
  // IN
  input [BITSIZE_in1-1:0] in1;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  generate
  if (BITSIZE_out1 <= BITSIZE_in1)
  begin
    assign out1 = in1[BITSIZE_out1-1:0];
  end
  else
  begin
    assign out1 = {{(BITSIZE_out1-BITSIZE_in1){1'b0}},in1};
  end
  endgenerate
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>, Christian Pilato <christian.pilato@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module MUX_GATE(sel, in1, in2, out1);
  parameter BITSIZE_in1=1, BITSIZE_in2=1, BITSIZE_out1=1;
  // IN
  input sel;
  input [BITSIZE_in1-1:0] in1;
  input [BITSIZE_in2-1:0] in2;
  // OUT
  output [BITSIZE_out1-1:0] out1;
  assign out1 = sel ? in1 : in2;
endmodule

// Datapath RTL description for addition
// This component has been derived from the input source code and so it does not fall under the copyright of PandA framework, but it follows the input source code copyright, and may be aggregated with components of the BAMBU/PANDA IP LIBRARY.
// Author(s): Component automatically generated by bambu
// License: THIS COMPONENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
`timescale 1ns / 1ps
module datapath_addition(clock, reset, in_port_a, in_port_b, in_port_result, M_Rdata_ram, M_DataRdy, Min_oe_ram, Min_we_ram, Min_addr_ram, Min_Wdata_ram, Min_data_ram_size, Mout_oe_ram, Mout_we_ram, Mout_addr_ram, Mout_Wdata_ram, Mout_data_ram_size, selector_IN_UNBOUNDED_addition_28856_28881, selector_IN_UNBOUNDED_addition_28856_28882, selector_IN_UNBOUNDED_addition_28856_28884, fuselector_MEMORY_CTRL_D21_3_i0_LOAD, fuselector_MEMORY_CTRL_D21_3_i0_STORE, selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0, selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1, selector_MUX_4_iu_conv_conn_obj_0_0_0_0, wrenable_reg_0, wrenable_reg_1, wrenable_reg_2, OUT_UNBOUNDED_addition_28856_28881, OUT_UNBOUNDED_addition_28856_28882, OUT_UNBOUNDED_addition_28856_28884);
  // IN
  input clock;
  input reset;
  input [31:0] in_port_a;
  input [31:0] in_port_b;
  input [31:0] in_port_result;
  input [31:0] M_Rdata_ram;
  input M_DataRdy;
  input Min_oe_ram;
  input Min_we_ram;
  input [31:0] Min_addr_ram;
  input [31:0] Min_Wdata_ram;
  input [5:0] Min_data_ram_size;
  input selector_IN_UNBOUNDED_addition_28856_28881;
  input selector_IN_UNBOUNDED_addition_28856_28882;
  input selector_IN_UNBOUNDED_addition_28856_28884;
  input fuselector_MEMORY_CTRL_D21_3_i0_LOAD;
  input fuselector_MEMORY_CTRL_D21_3_i0_STORE;
  input selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0;
  input selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1;
  input selector_MUX_4_iu_conv_conn_obj_0_0_0_0;
  input wrenable_reg_0;
  input wrenable_reg_1;
  input wrenable_reg_2;
  // OUT
  output Mout_oe_ram;
  output Mout_we_ram;
  output [31:0] Mout_addr_ram;
  output [31:0] Mout_Wdata_ram;
  output [5:0] Mout_data_ram_size;
  output OUT_UNBOUNDED_addition_28856_28881;
  output OUT_UNBOUNDED_addition_28856_28882;
  output OUT_UNBOUNDED_addition_28856_28884;
  // Component and signal declarations
  wire [31:0] out_MEMORY_CTRL_D21_3_i0_MEMORY_CTRL_D21_3_i0;
  wire [31:0] out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0;
  wire [31:0] out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1;
  wire [31:0] out_MUX_4_iu_conv_conn_obj_0_0_0_0;
  wire [6:0] out_const_0;
  wire out_const_1;
  wire [5:0] out_conv_out_const_0_7_6;
  wire [31:0] out_iu_conv_conn_obj_0_IUdata_converter_FU_iu_conv_0;
  wire signed [31:0] out_plus_expr_FU_32_32_32_4_i0_fu_addition_28856_28883;
  wire [31:0] out_reg_0_reg_0;
  wire [31:0] out_reg_1_reg_1;
  wire [31:0] out_reg_2_reg_2;
  wire s_MEMORY_CTRL_D21_3_i00;
  wire s_done_MEMORY_CTRL_D21_3_i0;
  wire [31:0] sig_in_bus_mergerMout_Wdata_ram0_0;
  wire [31:0] sig_in_bus_mergerMout_addr_ram1_0;
  wire [5:0] sig_in_bus_mergerMout_data_ram_size2_0;
  wire sig_in_bus_mergerMout_oe_ram3_0;
  wire sig_in_bus_mergerMout_we_ram4_0;
  wire [31:0] sig_out_bus_mergerMout_Wdata_ram0_;
  wire [31:0] sig_out_bus_mergerMout_addr_ram1_;
  wire [5:0] sig_out_bus_mergerMout_data_ram_size2_;
  wire sig_out_bus_mergerMout_oe_ram3_;
  wire sig_out_bus_mergerMout_we_ram4_;
  
  IUdata_converter_FU #(.BITSIZE_in1(32), .BITSIZE_out1(32)) IUdata_converter_FU_iu_conv_0 (.out1(out_iu_conv_conn_obj_0_IUdata_converter_FU_iu_conv_0), .in1(out_MUX_4_iu_conv_conn_obj_0_0_0_0));
  MEMORY_CTRL_D21 #(.BITSIZE_in1(32), .BITSIZE_in2(32), .BITSIZE_in3(6), .BITSIZE_out1(32), .BITSIZE_Min_addr_ram(32), .BITSIZE_Mout_addr_ram(32), .BITSIZE_M_Rdata_ram(32), .BITSIZE_Min_Wdata_ram(32), .BITSIZE_Mout_Wdata_ram(32), .BITSIZE_Min_data_ram_size(6), .BITSIZE_Mout_data_ram_size(6)) MEMORY_CTRL_D21_3_i0 (.done_port(s_done_MEMORY_CTRL_D21_3_i0), .out1(out_MEMORY_CTRL_D21_3_i0_MEMORY_CTRL_D21_3_i0), .Mout_oe_ram(sig_in_bus_mergerMout_oe_ram3_0), .Mout_we_ram(sig_in_bus_mergerMout_we_ram4_0), .Mout_addr_ram(sig_in_bus_mergerMout_addr_ram1_0), .Mout_Wdata_ram(sig_in_bus_mergerMout_Wdata_ram0_0), .Mout_data_ram_size(sig_in_bus_mergerMout_data_ram_size2_0), .clock(clock), .start_port(s_MEMORY_CTRL_D21_3_i00), .in1(out_iu_conv_conn_obj_0_IUdata_converter_FU_iu_conv_0), .in2(out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1), .in3(out_conv_out_const_0_7_6), .in4(out_const_1), .sel_LOAD(fuselector_MEMORY_CTRL_D21_3_i0_LOAD), .sel_STORE(fuselector_MEMORY_CTRL_D21_3_i0_STORE), .Min_oe_ram(Min_oe_ram), .Min_we_ram(Min_we_ram), .Min_addr_ram(Min_addr_ram), .M_Rdata_ram(M_Rdata_ram), .Min_Wdata_ram(Min_Wdata_ram), .Min_data_ram_size(Min_data_ram_size), .M_DataRdy(M_DataRdy));
  MUX_GATE #(.BITSIZE_in1(32), .BITSIZE_in2(32), .BITSIZE_out1(32)) MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 (.out1(out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0), .sel(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0), .in1(in_port_a), .in2(in_port_b));
  MUX_GATE #(.BITSIZE_in1(32), .BITSIZE_in2(32), .BITSIZE_out1(32)) MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 (.out1(out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1), .sel(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1), .in1(in_port_result), .in2(out_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0));
  MUX_GATE #(.BITSIZE_in1(32), .BITSIZE_in2(32), .BITSIZE_out1(32)) MUX_4_iu_conv_conn_obj_0_0_0_0 (.out1(out_MUX_4_iu_conv_conn_obj_0_0_0_0), .sel(selector_MUX_4_iu_conv_conn_obj_0_0_0_0), .in1(out_reg_2_reg_2), .in2(out_plus_expr_FU_32_32_32_4_i0_fu_addition_28856_28883));
  bus_merger #(.BITSIZE_in1(32), .PORTSIZE_in1(1), .BITSIZE_out1(32)) bus_mergerMout_Wdata_ram0_ (.out1(sig_out_bus_mergerMout_Wdata_ram0_), .in1({sig_in_bus_mergerMout_Wdata_ram0_0}));
  bus_merger #(.BITSIZE_in1(32), .PORTSIZE_in1(1), .BITSIZE_out1(32)) bus_mergerMout_addr_ram1_ (.out1(sig_out_bus_mergerMout_addr_ram1_), .in1({sig_in_bus_mergerMout_addr_ram1_0}));
  bus_merger #(.BITSIZE_in1(6), .PORTSIZE_in1(1), .BITSIZE_out1(6)) bus_mergerMout_data_ram_size2_ (.out1(sig_out_bus_mergerMout_data_ram_size2_), .in1({sig_in_bus_mergerMout_data_ram_size2_0}));
  bus_merger #(.BITSIZE_in1(1), .PORTSIZE_in1(1), .BITSIZE_out1(1)) bus_mergerMout_oe_ram3_ (.out1(sig_out_bus_mergerMout_oe_ram3_), .in1({sig_in_bus_mergerMout_oe_ram3_0}));
  bus_merger #(.BITSIZE_in1(1), .PORTSIZE_in1(1), .BITSIZE_out1(1)) bus_mergerMout_we_ram4_ (.out1(sig_out_bus_mergerMout_we_ram4_), .in1({sig_in_bus_mergerMout_we_ram4_0}));
  constant_value #(.BITSIZE_out1(7), .value(7'b0100000)) const_0 (.out1(out_const_0));
  constant_value #(.BITSIZE_out1(1), .value(1'b1)) const_1 (.out1(out_const_1));
  UUdata_converter_FU #(.BITSIZE_in1(7), .BITSIZE_out1(6)) conv_out_const_0_7_6 (.out1(out_conv_out_const_0_7_6), .in1(out_const_0));
  plus_expr_FU #(.BITSIZE_in1(32), .BITSIZE_in2(32), .BITSIZE_out1(32)) fu_addition_28856_28883 (.out1(out_plus_expr_FU_32_32_32_4_i0_fu_addition_28856_28883), .in1(out_reg_0_reg_0), .in2(out_reg_1_reg_1));
  or or_or_MEMORY_CTRL_D21_3_i00( s_MEMORY_CTRL_D21_3_i00, selector_IN_UNBOUNDED_addition_28856_28881, selector_IN_UNBOUNDED_addition_28856_28882, selector_IN_UNBOUNDED_addition_28856_28884);
  register_SE #(.BITSIZE_in1(32), .BITSIZE_out1(32)) reg_0 (.out1(out_reg_0_reg_0), .clock(clock), .reset(reset), .in1(out_MEMORY_CTRL_D21_3_i0_MEMORY_CTRL_D21_3_i0), .wenable(wrenable_reg_0));
  register_SE #(.BITSIZE_in1(32), .BITSIZE_out1(32)) reg_1 (.out1(out_reg_1_reg_1), .clock(clock), .reset(reset), .in1(out_MEMORY_CTRL_D21_3_i0_MEMORY_CTRL_D21_3_i0), .wenable(wrenable_reg_1));
  register_SE #(.BITSIZE_in1(32), .BITSIZE_out1(32)) reg_2 (.out1(out_reg_2_reg_2), .clock(clock), .reset(reset), .in1(out_plus_expr_FU_32_32_32_4_i0_fu_addition_28856_28883), .wenable(wrenable_reg_2));
  // io-signal post fix
  assign Mout_oe_ram = sig_out_bus_mergerMout_oe_ram3_;
  assign Mout_we_ram = sig_out_bus_mergerMout_we_ram4_;
  assign Mout_addr_ram = sig_out_bus_mergerMout_addr_ram1_;
  assign Mout_Wdata_ram = sig_out_bus_mergerMout_Wdata_ram0_;
  assign Mout_data_ram_size = sig_out_bus_mergerMout_data_ram_size2_;
  assign OUT_UNBOUNDED_addition_28856_28881 = s_done_MEMORY_CTRL_D21_3_i0;
  assign OUT_UNBOUNDED_addition_28856_28882 = s_done_MEMORY_CTRL_D21_3_i0;
  assign OUT_UNBOUNDED_addition_28856_28884 = s_done_MEMORY_CTRL_D21_3_i0;

endmodule

// FSM based controller description for addition
// This component has been derived from the input source code and so it does not fall under the copyright of PandA framework, but it follows the input source code copyright, and may be aggregated with components of the BAMBU/PANDA IP LIBRARY.
// Author(s): Component automatically generated by bambu
// License: THIS COMPONENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
`timescale 1ns / 1ps
module controller_addition(done_port, selector_IN_UNBOUNDED_addition_28856_28881, selector_IN_UNBOUNDED_addition_28856_28882, selector_IN_UNBOUNDED_addition_28856_28884, fuselector_MEMORY_CTRL_D21_3_i0_LOAD, fuselector_MEMORY_CTRL_D21_3_i0_STORE, selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0, selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1, selector_MUX_4_iu_conv_conn_obj_0_0_0_0, wrenable_reg_0, wrenable_reg_1, wrenable_reg_2, OUT_UNBOUNDED_addition_28856_28881, OUT_UNBOUNDED_addition_28856_28882, OUT_UNBOUNDED_addition_28856_28884, clock, reset, start_port);
  // IN
  input OUT_UNBOUNDED_addition_28856_28881;
  input OUT_UNBOUNDED_addition_28856_28882;
  input OUT_UNBOUNDED_addition_28856_28884;
  input clock;
  input reset;
  input start_port;
  // OUT
  output done_port;
  output selector_IN_UNBOUNDED_addition_28856_28881;
  output selector_IN_UNBOUNDED_addition_28856_28882;
  output selector_IN_UNBOUNDED_addition_28856_28884;
  output fuselector_MEMORY_CTRL_D21_3_i0_LOAD;
  output fuselector_MEMORY_CTRL_D21_3_i0_STORE;
  output selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0;
  output selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1;
  output selector_MUX_4_iu_conv_conn_obj_0_0_0_0;
  output wrenable_reg_0;
  output wrenable_reg_1;
  output wrenable_reg_2;
  parameter [6:0] S_0 = 7'b0000001,
    S_1 = 7'b0000010,
    S_2 = 7'b0000100,
    S_3 = 7'b0001000,
    S_4 = 7'b0010000,
    S_5 = 7'b0100000,
    S_6 = 7'b1000000;
  reg [6:0] _present_state, _next_state;
  reg done_port;
  reg selector_IN_UNBOUNDED_addition_28856_28881;
  reg selector_IN_UNBOUNDED_addition_28856_28882;
  reg selector_IN_UNBOUNDED_addition_28856_28884;
  reg fuselector_MEMORY_CTRL_D21_3_i0_LOAD;
  reg fuselector_MEMORY_CTRL_D21_3_i0_STORE;
  reg selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0;
  reg selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1;
  reg selector_MUX_4_iu_conv_conn_obj_0_0_0_0;
  reg wrenable_reg_0;
  reg wrenable_reg_1;
  reg wrenable_reg_2;
  
  always @(posedge clock)
    if (reset == 1'b0) _present_state <= S_0;
    else _present_state <= _next_state;
  
  always @(*)
  begin
    done_port = 1'b0;
    selector_IN_UNBOUNDED_addition_28856_28881 = 1'b0;
    selector_IN_UNBOUNDED_addition_28856_28882 = 1'b0;
    selector_IN_UNBOUNDED_addition_28856_28884 = 1'b0;
    fuselector_MEMORY_CTRL_D21_3_i0_LOAD = 1'b0;
    fuselector_MEMORY_CTRL_D21_3_i0_STORE = 1'b0;
    selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 = 1'b0;
    selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 = 1'b0;
    selector_MUX_4_iu_conv_conn_obj_0_0_0_0 = 1'b0;
    wrenable_reg_0 = 1'b0;
    wrenable_reg_1 = 1'b0;
    wrenable_reg_2 = 1'b0;
    case (_present_state)
      S_0 :
        if(start_port == 1'b1)
        begin
          selector_IN_UNBOUNDED_addition_28856_28881 = 1'b1;
          fuselector_MEMORY_CTRL_D21_3_i0_LOAD = 1'b1;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 = 1'b1;
          wrenable_reg_0 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28881 == 1'b0)
            begin
              _next_state = S_1;
            end
          else
            begin
              _next_state = S_2;
            end
        end
        else
        begin
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 = 1'bX;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 = 1'bX;
          selector_MUX_4_iu_conv_conn_obj_0_0_0_0 = 1'bX;
          wrenable_reg_0 = 1'bX;
          wrenable_reg_1 = 1'bX;
          wrenable_reg_2 = 1'bX;
          _next_state = S_0;
        end
      S_1 :
        begin
          fuselector_MEMORY_CTRL_D21_3_i0_LOAD = 1'b1;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 = 1'b1;
          wrenable_reg_0 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28881 == 1'b0)
            begin
              _next_state = S_1;
            end
          else
            begin
              _next_state = S_2;
            end
        end
      S_2 :
        begin
          selector_IN_UNBOUNDED_addition_28856_28882 = 1'b1;
          fuselector_MEMORY_CTRL_D21_3_i0_LOAD = 1'b1;
          wrenable_reg_1 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28882 == 1'b0)
            begin
              _next_state = S_3;
            end
          else
            begin
              _next_state = S_4;
            end
        end
      S_3 :
        begin
          fuselector_MEMORY_CTRL_D21_3_i0_LOAD = 1'b1;
          wrenable_reg_1 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28882 == 1'b0)
            begin
              _next_state = S_3;
            end
          else
            begin
              _next_state = S_4;
            end
        end
      S_4 :
        begin
          selector_IN_UNBOUNDED_addition_28856_28884 = 1'b1;
          fuselector_MEMORY_CTRL_D21_3_i0_STORE = 1'b1;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 = 1'b1;
          wrenable_reg_2 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28884 == 1'b0)
            begin
              _next_state = S_5;
            end
          else
            begin
              _next_state = S_6;
              done_port = 1'b1;
            end
        end
      S_5 :
        begin
          fuselector_MEMORY_CTRL_D21_3_i0_STORE = 1'b1;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 = 1'b1;
          selector_MUX_4_iu_conv_conn_obj_0_0_0_0 = 1'b1;
          if (OUT_UNBOUNDED_addition_28856_28884 == 1'b0)
            begin
              _next_state = S_5;
            end
          else
            begin
              _next_state = S_6;
              done_port = 1'b1;
            end
        end
      S_6 :
        begin
          _next_state = S_0;
        end
      default :
        begin
          _next_state = S_0;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0 = 1'bX;
          selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1 = 1'bX;
          selector_MUX_4_iu_conv_conn_obj_0_0_0_0 = 1'bX;
          wrenable_reg_0 = 1'bX;
          wrenable_reg_1 = 1'bX;
          wrenable_reg_2 = 1'bX;
        end
    endcase
  end
endmodule

// This component is part of the BAMBU/PANDA IP LIBRARY
// Copyright (C) 2004-2020 Politecnico di Milano
// Author(s): Marco Lattuada <marco.lattuada@polimi.it>
// License: PANDA_LGPLv3
`timescale 1ns / 1ps
module flipflop_AR(clock, reset, in1, out1);
  parameter BITSIZE_in1=1, BITSIZE_out1=1;
  // IN
  input clock;
  input reset;
  input in1;
  // OUT
  output out1;
  
  reg reg_out1 =0;
  assign out1 = reg_out1;
  always @(posedge clock )
    if (reset == 1'b0)
      reg_out1 <= {BITSIZE_out1{1'b0}};
    else
      reg_out1 <= in1;
endmodule

// Top component for addition
// This component has been derived from the input source code and so it does not fall under the copyright of PandA framework, but it follows the input source code copyright, and may be aggregated with components of the BAMBU/PANDA IP LIBRARY.
// Author(s): Component automatically generated by bambu
// License: THIS COMPONENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
`timescale 1ns / 1ps
module _addition(clock, reset, start_port, done_port, a, b, result, M_Rdata_ram, M_DataRdy, Min_oe_ram, Min_we_ram, Min_addr_ram, Min_Wdata_ram, Min_data_ram_size, Mout_oe_ram, Mout_we_ram, Mout_addr_ram, Mout_Wdata_ram, Mout_data_ram_size);
  // IN
  input clock;
  input reset;
  input start_port;
  input [31:0] a;
  input [31:0] b;
  input [31:0] result;
  input [31:0] M_Rdata_ram;
  input M_DataRdy;
  input Min_oe_ram;
  input Min_we_ram;
  input [31:0] Min_addr_ram;
  input [31:0] Min_Wdata_ram;
  input [5:0] Min_data_ram_size;
  // OUT
  output done_port;
  output Mout_oe_ram;
  output Mout_we_ram;
  output [31:0] Mout_addr_ram;
  output [31:0] Mout_Wdata_ram;
  output [5:0] Mout_data_ram_size;
  // Component and signal declarations
  wire OUT_UNBOUNDED_addition_28856_28881;
  wire OUT_UNBOUNDED_addition_28856_28882;
  wire OUT_UNBOUNDED_addition_28856_28884;
  wire done_delayed_REG_signal_in;
  wire done_delayed_REG_signal_out;
  wire fuselector_MEMORY_CTRL_D21_3_i0_LOAD;
  wire fuselector_MEMORY_CTRL_D21_3_i0_STORE;
  wire selector_IN_UNBOUNDED_addition_28856_28881;
  wire selector_IN_UNBOUNDED_addition_28856_28882;
  wire selector_IN_UNBOUNDED_addition_28856_28884;
  wire selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0;
  wire selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1;
  wire selector_MUX_4_iu_conv_conn_obj_0_0_0_0;
  wire wrenable_reg_0;
  wire wrenable_reg_1;
  wire wrenable_reg_2;
  
  controller_addition Controller_i (.done_port(done_delayed_REG_signal_in), .selector_IN_UNBOUNDED_addition_28856_28881(selector_IN_UNBOUNDED_addition_28856_28881), .selector_IN_UNBOUNDED_addition_28856_28882(selector_IN_UNBOUNDED_addition_28856_28882), .selector_IN_UNBOUNDED_addition_28856_28884(selector_IN_UNBOUNDED_addition_28856_28884), .fuselector_MEMORY_CTRL_D21_3_i0_LOAD(fuselector_MEMORY_CTRL_D21_3_i0_LOAD), .fuselector_MEMORY_CTRL_D21_3_i0_STORE(fuselector_MEMORY_CTRL_D21_3_i0_STORE), .selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0), .selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1), .selector_MUX_4_iu_conv_conn_obj_0_0_0_0(selector_MUX_4_iu_conv_conn_obj_0_0_0_0), .wrenable_reg_0(wrenable_reg_0), .wrenable_reg_1(wrenable_reg_1), .wrenable_reg_2(wrenable_reg_2), .OUT_UNBOUNDED_addition_28856_28881(OUT_UNBOUNDED_addition_28856_28881), .OUT_UNBOUNDED_addition_28856_28882(OUT_UNBOUNDED_addition_28856_28882), .OUT_UNBOUNDED_addition_28856_28884(OUT_UNBOUNDED_addition_28856_28884), .clock(clock), .reset(reset), .start_port(start_port));
  datapath_addition Datapath_i (.Mout_oe_ram(Mout_oe_ram), .Mout_we_ram(Mout_we_ram), .Mout_addr_ram(Mout_addr_ram), .Mout_Wdata_ram(Mout_Wdata_ram), .Mout_data_ram_size(Mout_data_ram_size), .OUT_UNBOUNDED_addition_28856_28881(OUT_UNBOUNDED_addition_28856_28881), .OUT_UNBOUNDED_addition_28856_28882(OUT_UNBOUNDED_addition_28856_28882), .OUT_UNBOUNDED_addition_28856_28884(OUT_UNBOUNDED_addition_28856_28884), .clock(clock), .reset(reset), .in_port_a(a), .in_port_b(b), .in_port_result(result), .M_Rdata_ram(M_Rdata_ram), .M_DataRdy(M_DataRdy), .Min_oe_ram(Min_oe_ram), .Min_we_ram(Min_we_ram), .Min_addr_ram(Min_addr_ram), .Min_Wdata_ram(Min_Wdata_ram), .Min_data_ram_size(Min_data_ram_size), .selector_IN_UNBOUNDED_addition_28856_28881(selector_IN_UNBOUNDED_addition_28856_28881), .selector_IN_UNBOUNDED_addition_28856_28882(selector_IN_UNBOUNDED_addition_28856_28882), .selector_IN_UNBOUNDED_addition_28856_28884(selector_IN_UNBOUNDED_addition_28856_28884), .fuselector_MEMORY_CTRL_D21_3_i0_LOAD(fuselector_MEMORY_CTRL_D21_3_i0_LOAD), .fuselector_MEMORY_CTRL_D21_3_i0_STORE(fuselector_MEMORY_CTRL_D21_3_i0_STORE), .selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_0), .selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1(selector_MUX_1_MEMORY_CTRL_D21_3_i0_1_0_1), .selector_MUX_4_iu_conv_conn_obj_0_0_0_0(selector_MUX_4_iu_conv_conn_obj_0_0_0_0), .wrenable_reg_0(wrenable_reg_0), .wrenable_reg_1(wrenable_reg_1), .wrenable_reg_2(wrenable_reg_2));
  flipflop_AR #(.BITSIZE_in1(1), .BITSIZE_out1(1)) done_delayed_REG (.out1(done_delayed_REG_signal_out), .clock(clock), .reset(reset), .in1(done_delayed_REG_signal_in));
  // io-signal post fix
  assign done_port = done_delayed_REG_signal_out;

endmodule

// Minimal interface for function: addition
// This component has been derived from the input source code and so it does not fall under the copyright of PandA framework, but it follows the input source code copyright, and may be aggregated with components of the BAMBU/PANDA IP LIBRARY.
// Author(s): Component automatically generated by bambu
// License: THIS COMPONENT IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
`timescale 1ns / 1ps
module addition(clock, reset, start_port, a, b, result, M_Rdata_ram, M_DataRdy, done_port, Mout_oe_ram, Mout_we_ram, Mout_addr_ram, Mout_Wdata_ram, Mout_data_ram_size);
  // IN
  input clock;
  input reset;
  input start_port;
  input [31:0] a;
  input [31:0] b;
  input [31:0] result;
  input [31:0] M_Rdata_ram;
  input M_DataRdy;
  // OUT
  output done_port;
  output Mout_oe_ram;
  output Mout_we_ram;
  output [31:0] Mout_addr_ram;
  output [31:0] Mout_Wdata_ram;
  output [5:0] Mout_data_ram_size;
  // Component and signal declarations
  
  _addition _addition_i0 (.done_port(done_port), .Mout_oe_ram(Mout_oe_ram), .Mout_we_ram(Mout_we_ram), .Mout_addr_ram(Mout_addr_ram), .Mout_Wdata_ram(Mout_Wdata_ram), .Mout_data_ram_size(Mout_data_ram_size), .clock(clock), .reset(reset), .start_port(start_port), .a(a), .b(b), .result(result), .M_Rdata_ram(M_Rdata_ram), .M_DataRdy(M_DataRdy), .Min_oe_ram(1'b0), .Min_we_ram(1'b0), .Min_addr_ram(32'b00000000000000000000000000000000), .Min_Wdata_ram(32'b00000000000000000000000000000000), .Min_data_ram_size(6'b000000));

endmodule


