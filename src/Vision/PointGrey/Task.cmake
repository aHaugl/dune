if(NOT DUNE_USING_OPENCV)
  set(TASK_ENABLED FALSE)
endif(NOT DUNE_USING_OPENCV)

if(NOT DUNE_USING_FLYCAPTURE)
  set(TASK_ENABLED FALSE)
endif(NOT DUNE_USING_FLYCAPTURE)

if(NOT DUNE_USING_EXIV2)
  set(TASK_ENABLED FALSE)
endif(NOT DUNE_USING_EXIV2)

set(TASK_LICENSE "Proprietary")
