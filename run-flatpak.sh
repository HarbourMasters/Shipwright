flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

flatpak-builder build -v --install-deps-from=flathub --force-clean --ccache --install com.shipofharkinian.soh.yaml

flatpak build-bundle /var/lib/flatpak/repo soh.flatpak com.shipofharkinian.soh Stable
