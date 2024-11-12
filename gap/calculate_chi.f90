module globals
    implicit none
    real(8), parameter :: pi = 3.1415926535897932384626433832795028841971
    integer :: nb, ne, nge(3), ngw(3), nke, nkw
    real(8) :: ef, qmesh(3), bvec(3,3), VBZ
    contains
        subroutine get_vals()
            bvec(1:3,1) = [3d0, 0d0, 0d0]
            bvec(1:3,2) = [0d0, 3d0, 0d0]
            bvec(1:3,3) = [0d0, 0d0, 3d0]
            nge = [10, 10, 10]
            ngw = [10, 10, 10]
            ef = 0.5d0
            ne = 10
            nb = 1
            qmesh = [10, 10, 10]
            VBZ = bvec(1,1) * bvec(2,2) * bvec(3,3) + bvec(1,2) * bvec(2,3) * bvec(3,1) &
            &   + bvec(1,3) * bvec(2,1) * bvec(3,2) - bvec(1,3) * bvec(2,2) * bvec(3,1) &
            &   + bvec(1,2) * bvec(2,1) * bvec(3,3) - bvec(1,1) * bvec(2,3) * bvec(3,2)
            nke = product(nge)
            nkw = product(ngw)
        end subroutine get_vals
end module globals
module mesh
    use globals
    use libtetrabz
    implicit none
    contains

    function get_kvec(i1, i2, i3) result(kvec)
        integer, intent(in) :: i1, i2, i3
        real, dimension(3) :: kvec
        kvec(1:3) = dble([i1, i2, i3]) / dble(nge(1:3))
    end function get_kvec

    function epsilon(kvec) result(eps)
        real, dimension(3), intent(in) :: kvec
        real :: eps
        eps = 0.5d0 * dot_product(kvec(1:3), kvec(1:3))
    end function epsilon

    function fill_energy_mesh(qvec) result(eig)
        real(8), dimension(3), intent(in) :: qvec
        integer :: i1, i2, i3, ik
        real, dimension(3) :: kvec
        real(8), dimension(nb,nke) :: eig
        ik = 0
        do i3 = 0, nge(3) - 1
            do i2 = 0, nge(2) - 1
                do i1 = 0, nge(1) - 1
                    ik = ik + 1
                    kvec = get_kvec(i1, i2, i3)
                    kvec(1:3) = matmul(bvec(1:3,1:3), kvec(1:3))
                    kvec(1:3) = kvec(1:3) + qvec(1:3)
                    eig(1,ik) = epsilon(kvec) - ef

                end do
            end do
        end do
     end function fill_energy_mesh

    function get_static_polarization(qvec, eig1, eig2, wght) result(chi)
        integer :: ltetra = 2
        real(8) :: eig1(:,:), qvec(3)
        real(8) :: eig2(:,:), wght(:,:,:)
        real(8) :: chi

        ltetra = 2
        eig2 = fill_energy_mesh(qvec)

        ! Calculate the polarization function
        call libtetrabz_polstat(ltetra,bvec,nb,nge,eig1,eig2,ngw,wght)
        chi = 2d0 * sum(wght(1:nb,1:nb,1:nkw)) * VBZ
    end function get_static_polarization

     function get_static_polarization_mesh() result(chi_mesh)
        real(8) :: chi, chi_mesh(nge(1), nge(2), nge(3))
        integer :: i1, i2, i3
        real(8), dimension(3) :: qvec
        real(8), allocatable :: eig1(:,:), eig2(:,:), wght(:,:,:)
        allocate(eig1(nb,nke), eig2(nb,nke), wght(nb,nb,nkw))
        eig1 = fill_energy_mesh([0d0, 0d0, 0d0])
        do i3 = 0, nge(3) - 1
            do i2 = 0, nge(2) - 1
                do i1 = 0, nge(1) - 1
                    qvec = get_kvec(i1, i2, i3)
                    eig2 = fill_energy_mesh(qvec)
                    chi = get_static_polarization(qvec, eig1, eig2, wght)
                    chi_mesh(i1+1, i2+1, i3+1) = chi
                end do
            end do
        end do
        deallocate(eig1, eig2, wght)
     end function get_static_polarization_mesh

     subroutine save_mesh(chi_mesh)
        real(8) :: chi_mesh(nge(1), nge(2), nge(3))
        integer :: i1, i2, i3
        real, dimension(3) :: qvec
        open(10, file='chi_mesh.dat', status='unknown')
        do i3 = 0, nge(3) - 1
            do i2 = 0, nge(2) - 1
                do i1 = 0, nge(1) - 1
                    qvec = get_kvec(i1, i2, i3)
                    write(10, *) qvec, chi_mesh(i1+1, i2+1, i3+1)
                end do
            end do
        end do
        close(10)
     end subroutine save_mesh
 end module mesh
program main
    use globals
    use mesh
    implicit none
    real(8) :: qvec(3), chi
    real(8), allocatable :: eig1(:,:)
    real(8), allocatable :: chi_mesh(:,:,:)
    call get_vals()
    chi_mesh = get_static_polarization_mesh()
    call save_mesh(chi_mesh)
end program main
