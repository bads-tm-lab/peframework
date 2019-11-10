#include <peframework.h>

extern void PEFILE_BASIC_TESTS( void );
extern void PEFILE_DESERIALIZE_X64_TESTS( void );
extern void PEFILE_DESERIALIZE_ARM_TESTS( void );
extern void PEFILE_DESERIALIZE_ARM64_TESTS( void );
extern void PEFILE_SERIALIZE_TESTS( void );

int main( int argc, char *argv[] )
{
    PEFILE_BASIC_TESTS();
    PEFILE_DESERIALIZE_X64_TESTS();
    PEFILE_DESERIALIZE_ARM_TESTS();
    PEFILE_DESERIALIZE_ARM64_TESTS();
    PEFILE_SERIALIZE_TESTS();

    return 0;
}