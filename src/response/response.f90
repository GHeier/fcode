module response
    use confighub
    use globals
    use mesh
    implicit none
    contains
     subroutine polarization_wrapper() bind(C)
        real(8), ALLOCATABLE :: chi_mesh(:,:,:)
        call load_f90_config()
        call get_vals()
        allocate(chi_mesh(nge(1), nge(2), nge(3)))
        chi_mesh = get_static_polarization_mesh()
        call save_mesh(chi_mesh)
     end subroutine polarization_wrapper
 end module response