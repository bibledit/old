
<textarea name="area1" class="fullwidth"></textarea>

<script type="text/javascript" src="nicEdit.js"></script>

<script type="text/javascript">
	bkLib.onDomLoaded(function() { nicEditors.allTextAreas() });
</script>

<script type="text/javascript">
  $ ("body").on ("paste", function (e) {
console.log ("paste");
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    document.execCommand ("insertHTML", false, data);
  });

</script>
