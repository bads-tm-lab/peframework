#include <peframework.h>

extern void PEFILE_BASIC_TESTS( void );
extern void PEFILE_DESERIALIZE_TESTS( void );
extern void PEFILE_SERIALIZE_TESTS( void );

int main( int argc, char *argv[] )
{
    PEFILE_BASIC_TESTS();
    PEFILE_DESERIALIZE_TESTS();
    PEFILE_SERIALIZE_TESTS();

    return 0;
}