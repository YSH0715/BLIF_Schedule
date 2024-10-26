module sample;
input a;
input b;
input c;
input d;
input e;
input f;

output o;
output p;
output q;

wire g;
wire h;
wire i;
wire j;
wire k;
wire l;
wire m;
wire n;
wire o;
wire p;
wire q;

assign g=a|d;
assign h=a&c;
assign i=!c;
assign j=d|e|f;
assign k=g|h|i;
assign l=h&i&j;
assign m=i&j;
assign n=l&m;
assign o=b&h&k;
assign p=!g;
assign q=!n;

endmodule
