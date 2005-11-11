#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkShiftScaleImageFilter.h"
#include "itkTimeProbe.h"
#include "itkNumericTraits.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"

int main(int, char * argv[])
{
  const int dim = 3;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::InvertIntensityImageFilter< IType, IType > InvertType;
  InvertType::Pointer invert = InvertType::New();
  invert->SetInput( reader->GetOutput() );

  typedef itk::ShiftScaleImageFilter< IType, IType > ShiftScaleType;
  ShiftScaleType::Pointer shiftScale = ShiftScaleType::New();
  shiftScale->SetInput( reader->GetOutput() );
  shiftScale->SetShift( - itk::NumericTraits< PType >::max() );
  shiftScale->SetScale( -1 );

  typedef itk::IntensityWindowingImageFilter< IType, IType > WindowType;
  WindowType::Pointer window = WindowType::New();
  window->SetInput( reader->GetOutput() );
  window->SetOutputMinimum( itk::NumericTraits< PType >::max() );
  window->SetOutputMaximum( itk::NumericTraits< PType >::NonpositiveMin() );

  itk::TimeProbe iTime;
  itk::TimeProbe ssTime;
  itk::TimeProbe wTime;

  invert->SetNumberOfThreads( 1 );
  shiftScale->SetNumberOfThreads( 1 );
  window->SetNumberOfThreads( 1 );
  
  for( int i=0; i<100; i++ )
    {
    iTime.Start();
    invert->Update();
    iTime.Stop();
    
    ssTime.Start();
    shiftScale->Update();
    ssTime.Stop();
    
    wTime.Start();
    window->Update();
    wTime.Stop();
    
    invert->Modified();
    shiftScale->Modified();
    window->Modified();
    }
    
  std::cout << iTime.GetMeanTime() << "\t" << ssTime.GetMeanTime() << "\t" << wTime.GetMeanTime() << std::endl;
  
  return 0;
}

