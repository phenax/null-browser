(() => {
  /** @typedef {{ key: string; elem: HTMLElement; labelElem: HTMLElement | null }} Match */

  const addRoot = () => {
    const root = document.createElement('div');
    document.body.appendChild(root);
    return root;
  }

  /**
   * @param match {Match}
   * @return {Promise<HTMLElement | null>}
   */
  const createLabelElem = (match) => new Promise((resolve, reject) => {
    const label = Object.assign(document.createElement('div'), {
      className: '__nullbrowser_findlabel',
      textContent: match.key,
    });
    requestAnimationFrame(() => {
      const rect = match.elem.getBoundingClientRect();
      if (rect.top < 0 | rect.top > window.innerHeight) return reject(null);
      if (rect.left < 0 | rect.left > window.innerWidth) return reject(null);

      resolve(label);
      requestAnimationFrame(() => {
        Object.assign(label.style, {
          position: 'fixed',
          zIndex: 999999999, // maybe a few more 9s will do
          top: `${rect.top}px`,
          left: `${rect.left}px`,
          backgroundColor: 'yellow',
          border: '1px solid #000',
          boxShadow: '0 0 3px #555',
          color: '#222',
          padding: '1px 2px',
          fontSize: '10px',
          fontWeight: 'bold',
          fontFamily: 'monospace',
        });
      });
    });
  });

  const finder = {
    /** @type {string} */
    keys: '',
    /** @type {Array<Match>} */
    matches: [],
    /** @type {HTMLElement | null} */
    labelsRoot: null,
    /** @type {boolean} */
    openInNewView: false,

    stop() {
      finder.matches = [];
      finder.keys = '';
      [...(finder.labelsRoot?.children ?? [])].forEach(child => {
        child.remove();
      });
    },

    /**
     * @param selector {string}
     * @param new_view {boolean}
     */
    start(selector, new_view) {
      finder.stop()
      finder.labelsRoot ??= addRoot();
      finder.createMatches(selector);
      finder.openInNewView = new_view;
    },

    /** @param char {string} */
    filterOutByKey(char) {
      if (!/[0-9]+/.test(char)) return; // NOTE: temporary

      finder.keys = finder.keys + char;
      finder.matches = finder.matches.filter(m => {
        const isMatch = m.key.startsWith(finder.keys);
        if (!isMatch) {
          m.labelElem?.remove();
          return false;
        }
        return true;
      });
      if (finder.matches.length <= 1) {
        const match = finder.matches[0]
        finder.stop();
        if (match) {
          if (match.elem?.href) {
            if (finder.openInNewView) {
              window.open(match.elem?.href)
            } else {
              location.href = match.elem?.href
            }
          } else {
            match.elem?.click()
          }
        }
      }
    },

    /** @param selector {string} */
    createMatches(selector) {
      finder.matches = [];
      const elements = document.querySelectorAll(selector);

      const matches = [...elements].map(async (elem, index) => {
        /** @type {Match} */
        const match = {
          elem,
          key: `${index}`.padStart(`${elements.length}`.length, '0'),
          labelElem: null,
        }
        match.labelElem = await createLabelElem(match);
        return match
      });

      Promise.allSettled(matches).then(matches => {
        matches.forEach(result => {
          if (result.status !== 'fulfilled') return;
          if (result.value.labelElem)
            finder.labelsRoot.appendChild(result.value.labelElem)
          finder.matches.push(result.value)
        });
      });
    },
  };

  window._nullbrowser ||= {};
  window._nullbrowser.finder ||= finder;
})();
