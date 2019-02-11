#include <peframework.h>

#include <sdk/NumericFormat.h>

void PEFILE_BASIC_TESTS( void )
{
    printf( "testing PEFile construction..." );
    {
        PEFile image;

        assert( image.HasRelocationInfo() == false );
        assert( image.HasLinenumberInfo() == false );
        assert( image.HasDebugInfo() == false );
        assert( image.IsDynamicLinkLibrary() == false );

        assert( image.GetSectionCount() == 0 );

        // By default we create PE32 images.
        assert( image.isExtendedFormat == false );
    }
    printf( "ok.\n" );

    printf( "testing PEFile add section..." );
    {
        PEFile image;
        {
            PEFile::PESection section;
            section.shortName = ".meow";
            section.stream.WriteUInt32( 0x12345678 );
            section.Finalize();

            image.AddSection( std::move( section ) );

            assert( image.GetSectionCount() == 1 );
        }
        for ( auto iter = image.GetSectionIterator(); !iter.IsEnd(); iter.Increment() )
        {
            PEFile::PESection *theSect = iter.Resolve();

            assert( theSect->shortName == ".meow" );
            assert( theSect->stream.Size() == 4 );

            theSect->stream.Seek( 0 );
            assert( theSect->stream.ReadUInt32_EX() == 0x12345678 );
        }
    }
    printf( "ok.\n" );

    printf( "testing PEFile add empty section..." );
    {
        PEFile image;

        PEFile::PESection section;
        section.Finalize();

        image.AddSection( std::move( section ) );

        // Empty sections are actually invalid inside PE images.
        assert( image.GetSectionCount() == 0 );
    }
    printf( "ok.\n" );

    printf( "testing PEFile place section..." );
    {
        PEFile image;
        
        PEFile::PESection section;
        section.shortName = ".placed";
        section.stream.WriteUInt32( 0x12345678 );
        section.Finalize();
        section.SetPlacementInfo( 0x6000, section.GetVirtualSize() );

        image.PlaceSection( std::move( section ) );

        assert( image.GetSectionCount() == 1 );
    }
    printf( "ok.\n" );

    printf( "testing PEFile add many sections..." );
    {
        PEFile image;

        static constexpr size_t NUM_ADD_SECT = 10;

        for ( size_t n = 0; n < NUM_ADD_SECT; n++ )
        {
            PEFile::PESection section;
            section.shortName += ".sect";
            section.shortName += eir::to_string <char, PEGlobalStaticAllocator> ( n );
            section.stream.WriteUInt8( (uint8_t)n );
            section.Finalize();
           
            image.AddSection( std::move( section ) );
        }

        assert( image.GetSectionCount() == NUM_ADD_SECT );

        size_t n = 0;

        for ( auto iter = image.GetSectionIterator(); !iter.IsEnd(); iter.Increment(), n++ )
        {
            PEFile::PESection *theSect = iter.Resolve();

            auto expShortName = ( peString <char> ( ".sect" ) + eir::to_string <char, PEGlobalStaticAllocator> ( n ) );

            assert( theSect->shortName == expShortName );
        }
    }
    printf( "ok.\n" );

    printf( "testing PEFile fetch RVA into section..." );
    {
        PEFile image;

        // First add some sections.
        {
            static const char test_data[] = "hello this is a nice pe file section content pls read it";

            PEFile::PESection theSection;
            theSection.shortName = "test";
            theSection.stream.Write( test_data, sizeof(test_data) );
            theSection.Finalize();
            theSection.SetPlacementInfo( 0x10000, theSection.GetVirtualSize() );

            image.PlaceSection( std::move( theSection ) );

            PEFile::PESection otherSection;
            otherSection.shortName = "zonk";
            otherSection.stream.WriteUInt32( 0xcafebabe );
            otherSection.Finalize();

            image.AddSection( std::move( otherSection ) );
        }

        // Now query by the RVA.
        {
            PEFile::PESection *theSect = image.FindSectionByRVA( 0x10000 );

            assert( theSect != nullptr );
            assert( theSect->shortName == "test" );

            // Also inside the section should work.
            assert( image.FindSectionByRVA( 0x10001 ) == theSect );
            assert( image.FindSectionByRVA( 0x10010 ) == theSect );

            // Query RVA offsets.
            assert( theSect->ResolveRVA( 0x10 ) == 0x10010 );
        }
    }
    printf( "ok.\n" );
}