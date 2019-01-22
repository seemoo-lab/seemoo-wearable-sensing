/*global tau */
/*jslint unparam: true */
(function(tau) {
	var page,
		elScroller,
		list,
		headerHelper,
		listHelper = [],
		snapList = [],
		headerExpandHandler = [],
		headerCollapseHandler = [],
		i, len;

	if (tau.support.shape.circle) {
		document.addEventListener("pagebeforeshow", function (e) {
			page = e.target;
			elScroller = page.querySelector(".ui-scroller");

			if (elScroller) {
				list = elScroller.querySelectorAll(".ui-listview");
				if (list) {
					len = list.length;
					for (i = 0; i < len; i++) {
						listHelper[i] = tau.helper.SnapListStyle.create(list[i]);
					}
					len = listHelper.length;
					if (len) {
						for (i = 0; i < len; i++) {
							snapList[i] = listHelper[i].getSnapList();
							headerCollapseHandler[i] = snapList[i].enable.bind(snapList[i]);
							headerExpandHandler[i] = snapList[i].disable.bind(snapList[i]);
							page.addEventListener("headercollapse", headerCollapseHandler[i], false);
							page.addEventListener("headerexpand", headerExpandHandler[i], false);
						}
					}
				}
				elScroller.setAttribute("tizen-circular-scrollbar", "");
			}
			headerHelper = tau.helper.HeaderMarqueeStyle.create(page, {});
		});

		document.addEventListener("pagebeforehide", function (e) {
			len = listHelper.length;
			headerHelper.destroy();
			headerHelper = null;
			if (len) {
				for (i = 0; i < len; i++) {
					listHelper[i].destroy();
					page.removeEventListener("headercollapse", headerCollapseHandler[i], false);
					page.removeEventListener("headerexpand", headerExpandHandler[i], false);
				}
				listHelper = [];
			}
			if(elScroller) {
				elScroller.removeAttribute("tizen-circular-scrollbar");
			}
		});
	}
}(tau));
