# Purpose, usage and working principle of BRAM in LoCod

In this document, we present the usage and purpose of BRAM in the LoCod project. 


<br>

## What is BRAM ? 

BRAM stands for block RAM (Random Access Memory). It is a memoy present in the FPGA part of the SoC that makes it possible to store larger data like tables or lookup tables to be directly accessible by the FPGA.

If we can't utilize BRAM in FPGA the design created explodes in size if we use big variables. It's because the FPGA is forced to utilize registers all arround the FPGA design. 

### 1. Purpose

The purpose of BRAM is to enable for direct fast access to big variables like look up tables (kernels...) without having to pass through axi bus to access DDR memory which is slower.


### 2. Usage

To utilize BRAM you can : 
* Create your variables locally in functions
* Create your variables in static

IMPORTANT : 
When utilizing BRAM sub-fonctions to accelerators no longer work !

To fix this you can inline your sub-functions.

Exemple :
```ruby
static table_in_BRAM[10] = {1,2,3,4,5,6,7,8,9,10};

static inline int sub_function(int arg){
    return(arg*arg)
}

void acc0(int *param, int *result){
    int other_var_in_BRAM=param;
    for(int i=0;i<10;i++){
        table_in_BRAM[i]=sub_function(table_in_BRAM[i]);
        other_var_in_BRAM+=table_in_BRAM[i]
    }
    result=other_var_in_BRAM;      
}
```

Other important notice ! 

If you use matrix and need to initialize them make sure you do it properly and don't use light definition exemple in demo/binomial_coefficients_bram/main.c :

```
//For CPU, it's not mandatory and will work but if you assign it like this it'll not work with FPGA ! (it will be considered the first LINE full of 0 and not the first COLUMN !)
unsigned int pascal_matrix_cpu[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE]={
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1}
};


//For FPGA Matrix need to by assigned precisely ! like this :
static unsigned int pascal_matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE]={

    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    
};
```



### 3. Principle of BRAM in LoCod

When we firstly wanted to utilize it we didn't make the link between sub function breaking BRAM. We were also having problems with the policy of data for bambu. 

To fix that and make it possible to use big vriables we forced panda bambu to store any variables in DDR memory (NO_BRAM). It's not fast but at least it worked for the time. 

We settled on LSS instead of NO_BRAM or ALL_BRAM.
LSS makes it so every local variable (for example inside a function) will be stored in BRAM. It also makes static variables use BRAM (it enables us to have global variables stored in BRAM (for example kernels)). It also stores strings inside BRAM. 

ALL_BRAM wasn't a good option because it would also store in BRAM our parameters and results from accelerators. With that policy we couldn't retrieve any data from the accelerators by reading it in DDR.

Then with further investigation we saw that BRAM was working when we weren't utilizing sub_functions. It's because panda bambu doesn't seem to know what a sub-function really needs in terms of variables. 

So to fix that we disabled optimisations with "-O0" option in bambu arguments. We also tested other optimisations and it dosen't work with them because the inline cannot be forced and the compiler chooses by itself if it inlines it or not. 

Inlining function may seem like a bad idea but it significantly improved performance. The most plausible hypothesis is that it parallelizes paths and reduces the length of the longest path (critical path) and speeds up the execution time. 

